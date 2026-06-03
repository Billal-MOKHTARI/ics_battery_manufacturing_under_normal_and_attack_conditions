import struct
import mysql.connector
from pymodbus.client import ModbusTcpClient
from datetime import datetime
import time
import logging
import os
import sys

# ANSI color codes
class Colors:
    RESET = '\033[0m'
    BOLD = '\033[1m'
    DIM = '\033[2m'
    
    # Foreground colors
    RED = '\033[31m'
    GREEN = '\033[32m'
    YELLOW = '\033[33m'
    BLUE = '\033[34m'
    MAGENTA = '\033[35m'
    CYAN = '\033[36m'
    WHITE = '\033[37m'
    
    # Bright foreground colors
    BRIGHT_RED = '\033[91m'
    BRIGHT_GREEN = '\033[92m'
    BRIGHT_YELLOW = '\033[93m'
    BRIGHT_BLUE = '\033[94m'
    BRIGHT_MAGENTA = '\033[95m'
    BRIGHT_CYAN = '\033[96m'
    BRIGHT_WHITE = '\033[97m'
    
    # Background colors
    BG_RED = '\033[41m'
    BG_GREEN = '\033[42m'
    BG_YELLOW = '\033[43m'
    BG_BLUE = '\033[44m'


# Configure logging for file only
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s',
    datefmt='%Y-%m-%d %H:%M:%S',
    filename='plc_logger.log',
    filemode='a'
)
logger = logging.getLogger(__name__)

# Suppress verbose pymodbus logs
logging.getLogger('pymodbus').setLevel(logging.WARNING)


def clear_console():
    """Clear console screen"""
    os.system('cls' if os.name == 'nt' else 'clear')


def colored(text, color):
    """Wrap text with color codes"""
    return f"{color}{text}{Colors.RESET}"


def create_session(cursor, modbus_ip, sql_ip, sql_port, plc_variable_id_prefix):
    """
    Create a new session for this program run.
    The interface name is resolved from the Modbus IP address.
    Returns session_id or None.
    """
    try:
        # Find the machine name from the PLCVariable prefix
        cursor.execute("""
            SELECT DISTINCT machine_id FROM PLCVariable 
            WHERE id LIKE %s LIMIT 1
        """, (f"{plc_variable_id_prefix}%",))
        machine_row = cursor.fetchone()
        
        if not machine_row:
            logger.error("[SESSION] No machine found for prefix: %s", plc_variable_id_prefix)
            print(colored("[ERROR] No machine found for the given prefix", Colors.RED))
            return None
        
        machine_name = machine_row[0]
        logger.info("[SESSION] Found machine: %s", machine_name)
        
        # Find the interface that matches the Modbus IP address
        cursor.execute("""
            SELECT i.machine, i.name, i.ipv4, i.mask, p.port
            FROM Interface i
            JOIN Port p ON i.machine = p.machine AND i.name = p.interface
            WHERE i.machine = %s AND i.ipv4 = %s
            LIMIT 1
        """, (machine_name, modbus_ip))
        session_info = cursor.fetchone()
        
        if not session_info:
            # If no exact match, try to find any interface for this machine
            logger.warning("[SESSION] No interface found with IP %s for machine %s, trying fallback", modbus_ip, machine_name)
            print(colored(f"[WARNING] No interface found with IP {modbus_ip}", Colors.YELLOW))
            print(colored(f"          Searching for any interface on machine '{machine_name}'...", Colors.YELLOW))
            
            cursor.execute("""
                SELECT i.machine, i.name, i.ipv4, i.mask, p.port
                FROM Interface i
                JOIN Port p ON i.machine = p.machine AND i.name = p.interface
                WHERE i.machine = %s
                LIMIT 1
            """, (machine_name,))
            session_info = cursor.fetchone()
            
            if not session_info:
                logger.error("[SESSION] No interface/port found for machine: %s", machine_name)
                print(colored("[ERROR] No interface/port configuration found for this machine", Colors.RED))
                return None
        
        machine, interface_name, ipv4, mask, port = session_info
        
        # Create the session with the resolved interface name
        cursor.execute("""
            INSERT INTO Session (machine, interface, port, 
                                 src_ipv4, src_mask, dest_ipv4, dest_port, 
                                 created_at, updated_at)
            VALUES (%s, %s, %s, %s, %s, %s, %s, NOW(), NOW())
        """, (machine, interface_name, port,
              ipv4, mask, sql_ip, sql_port))
        cursor._connection.commit()
        session_id = cursor.lastrowid
        
        logger.info("[SESSION] Created session %d - Machine: %s, Interface: %s, IP: %s, Port: %d",
                    session_id, machine, interface_name, ipv4, port)
        
        print(colored(f"[SESSION] Session {session_id} created successfully", Colors.BRIGHT_GREEN))
        print(colored(f"          Machine:   {machine}", Colors.CYAN))
        print(colored(f"          Interface: {interface_name} ({ipv4}/{mask})", Colors.CYAN))
        print(colored(f"          Port:      {port}", Colors.CYAN))
        
        return session_id
        
    except mysql.connector.Error as e:
        logger.error("[SESSION] Failed to create session: %s", e)
        print(colored(f"[ERROR] Failed to create session: {e}", Colors.RED))
        return None


def log_data(modbus_ip, modbus_port, cursor, history_sql_table, plc_variable_id_prefix, session_id):
    """
    Reads PLC variables from Modbus and logs them to a MySQL history table.
    Uses the provided session_id for all inserts.
    """
    
    # --- Step 1: Connect to Modbus ---
    try:
        client = ModbusTcpClient(modbus_ip, port=modbus_port)
        client.connect()
    except Exception as e:
        logger.error("[MODBUS] Connection failed: %s", e)
        print(colored(f"[ERROR] Modbus connection failed: {e}", Colors.RED))
        return 0

    # --- Step 2: Get history table columns ---
    try:
        cursor.execute(f"SHOW COLUMNS FROM {history_sql_table}")
        columns = cursor.fetchall()
        skip_columns = {'id', 'session', 'created_at', 'updated_at'}
        history_vars = []
        for col in columns:
            col_name = col[0]
            if col_name not in skip_columns and not col_name.endswith('_id'):
                history_vars.append(col_name)
    except mysql.connector.Error as e:
        logger.error("[MYSQL] Failed to get table columns: %s", e)
        print(colored(f"[ERROR] Failed to get table columns: {e}", Colors.RED))
        client.close()
        return 0

    if not history_vars:
        logger.warning("[MYSQL] No variable columns found")
        print(colored("[WARNING] No variable columns found in history table", Colors.YELLOW))
        client.close()
        return 0

    # --- Step 3: Get PLCVariable data ---
    try:
        query = """
            SELECT id, var, var_type, digital_address, pin_prog, physical_address
            FROM PLCVariable 
            WHERE id LIKE %s
        """
        cursor.execute(query, (f"{plc_variable_id_prefix}%",))
        plc_vars = cursor.fetchall()
        
        var_mapping = {}
        for row in plc_vars:
            plcv_id, var_name, var_type, digital_address, pin_prog, physical_address = row
            var_mapping[var_name] = {
                'id': plcv_id,
                'var_type': var_type,
                'digital_address': digital_address,
                'pin_prog': pin_prog,
                'physical_address': physical_address
            }
    except mysql.connector.Error as e:
        logger.error("[MYSQL] Failed to query PLCVariable: %s", e)
        print(colored(f"[ERROR] Failed to query PLCVariable: {e}", Colors.RED))
        client.close()
        return 0

    # --- Step 4: Read values from Modbus ---
    values = {}
    errors = 0
    success = 0
    
    sensors = []
    actuators = []
    states = []
    transitions = []
    
    for var_name in history_vars:
        if var_name not in var_mapping:
            continue
        
        var_info = var_mapping[var_name]
        pin_prog = var_info['pin_prog']
        var_type = var_info['var_type']
        address = var_info['digital_address']
        phys_addr = var_info['physical_address']
        
        try:
            if pin_prog == 'Coil':
                result = client.read_coils(address=address, count=1)
                if result.isError():
                    value = None
                    errors += 1
                else:
                    value = bool(result.bits[0])
                    success += 1
            
            elif pin_prog == 'Holding Register':
                result = client.read_holding_registers(address=address, count=1)
                if result.isError():
                    value = None
                    errors += 1
                else:
                    value = result.registers[0]
                    success += 1
            
            elif pin_prog == 'Memory':
                result = client.read_holding_registers(address=address, count=2)
                if result.isError():
                    value = None
                    errors += 1
                else:
                    packed = struct.pack('!HH', result.registers[0], result.registers[1])
                    value = round(struct.unpack('!f', packed)[0], 3)
                    success += 1
            else:
                value = None
                errors += 1
            
            values[var_name] = value
            
            entry = (var_name, phys_addr, value, var_type)
            if var_type == 'Sensor':
                sensors.append(entry)
            elif var_type == 'Actuator':
                actuators.append(entry)
            elif var_type == 'State':
                states.append(entry)
            elif var_type == 'Transition':
                transitions.append(entry)
                
        except Exception as e:
            logger.error("[MODBUS] Error reading %s: %s", var_name, e)
            values[var_name] = None
            errors += 1

    client.close()

    # --- Step 5: Display real-time values ---
    clear_console()
    
    current_time = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    
    # Header
    print()
    print(colored("=" * 80, Colors.CYAN))
    print(colored(f"  PLC REAL-TIME MONITOR - {history_sql_table}", Colors.BOLD + Colors.BRIGHT_WHITE))
    print(colored(f"  [MODBUS] {modbus_ip}:{modbus_port}  |  [SESSION] #{session_id}", Colors.DIM))
    status_color = Colors.BRIGHT_GREEN if errors == 0 else Colors.YELLOW
    print(colored(f"  [STATUS] Read: {success}  |  Errors: {errors}  |  Time: {current_time}", status_color))
    print(colored("=" * 80, Colors.CYAN))
    print()
    
    # Display Sensors
    if sensors:
        print(colored(f"  [SENSORS] ({len(sensors)} variables)", Colors.BOLD + Colors.BRIGHT_BLUE))
        print(colored(f"  " + "-" * 76, Colors.DIM))
        print(colored(f"  {'Variable':<20} {'Address':<14} {'Value':<16} {'Type':<12}", Colors.BRIGHT_WHITE))
        print(colored(f"  " + "-" * 76, Colors.DIM))
        for name, addr, val, vtype in sensors:
            if val is not None:
                if val is True:
                    val_color = Colors.BRIGHT_GREEN
                    val_str = "ON"
                elif val is False:
                    val_color = Colors.DIM
                    val_str = "OFF"
                else:
                    val_color = Colors.BRIGHT_WHITE
                    val_str = str(val)
                print(f"  {name:<20} {colored(addr, Colors.CYAN):<24} {colored(val_str, val_color):<26} {colored(vtype, Colors.MAGENTA):<12}")
        print()
    
    # Display Actuators
    if actuators:
        print(colored(f"  [ACTUATORS] ({len(actuators)} variables)", Colors.BOLD + Colors.BRIGHT_GREEN))
        print(colored(f"  " + "-" * 76, Colors.DIM))
        print(colored(f"  {'Variable':<20} {'Address':<14} {'Value':<16} {'Type':<12}", Colors.BRIGHT_WHITE))
        print(colored(f"  " + "-" * 76, Colors.DIM))
        for name, addr, val, vtype in actuators:
            if val is not None:
                if isinstance(val, float):
                    val_color = Colors.BRIGHT_YELLOW
                    val_str = f"{val:.3f}"
                elif val is True:
                    val_color = Colors.BRIGHT_GREEN
                    val_str = "ON"
                elif val is False:
                    val_color = Colors.DIM
                    val_str = "OFF"
                else:
                    val_color = Colors.BRIGHT_WHITE
                    val_str = str(val)
                print(f"  {name:<20} {colored(addr, Colors.CYAN):<24} {colored(val_str, val_color):<26} {colored(vtype, Colors.MAGENTA):<12}")
        print()
    
    # Display States
    if states:
        active_states = [s for s in states if s[2] is True]
        inactive_states = [s for s in states if s[2] is False]
        
        if active_states:
            print(colored(f"  [ACTIVE STATES] ({len(active_states)}):", Colors.BOLD + Colors.BRIGHT_MAGENTA), end=" ")
            for name, addr, val, vtype in active_states:
                print(colored(f"{name}", Colors.BRIGHT_MAGENTA), end=" ")
            print()
        
        if inactive_states:
            print(colored(f"  [INACTIVE STATES] ({len(inactive_states)}):", Colors.DIM), end=" ")
            shown = 0
            for name, addr, val, vtype in inactive_states:
                if shown < 5:
                    print(colored(f"{name}", Colors.DIM), end=" ")
                    shown += 1
            if len(inactive_states) > 5:
                print(colored(f"... +{len(inactive_states) - 5} more", Colors.DIM), end="")
            print()
        print()
    
    # Display Transitions
    if transitions:
        active_trans = [t for t in transitions if t[2] is True]
        inactive_trans = [t for t in transitions if t[2] is False]
        
        if active_trans:
            print(colored(f"  [ACTIVE TRANSITIONS] ({len(active_trans)}):", Colors.BOLD + Colors.BRIGHT_CYAN), end=" ")
            for name, addr, val, vtype in active_trans:
                print(colored(f"{name}", Colors.BRIGHT_CYAN), end=" ")
            print()
        
        if inactive_trans:
            print(colored(f"  [INACTIVE TRANSITIONS] ({len(inactive_trans)}):", Colors.DIM), end=" ")
            shown = 0
            for name, addr, val, vtype in inactive_trans:
                if shown < 3:
                    print(colored(f"{name}", Colors.DIM), end=" ")
                    shown += 1
            if len(inactive_trans) > 3:
                print(colored(f"... +{len(inactive_trans) - 3} more", Colors.DIM), end="")
            print()
        print()

    print(colored("  " + "-" * 76, Colors.DIM))
    print(colored("  Press CTRL+C to stop logging", Colors.DIM))
    print(colored("=" * 80, Colors.CYAN))
    print()

    # --- Step 6: Insert into history table ---
    if not values:
        return success

    try:
        columns = list(values.keys())
        placeholders = ', '.join(['%s'] * len(columns))
        column_names = ', '.join(columns)
        
        insert_query = f"""
            INSERT INTO {history_sql_table} 
            (session, {column_names}, created_at, updated_at)
            VALUES (%s, {placeholders}, NOW(), NOW())
        """
        
        insert_values = [session_id] + [values[col] for col in columns]
        cursor.execute(insert_query, insert_values)
        cursor._connection.commit()
        
        logger.info("[LOGGING] Data saved to %s (Session: %d, Values: %d)", history_sql_table, session_id, len(values))
        
        return success
        
    except mysql.connector.Error as e:
        logger.error("[MYSQL] Insert failed: %s", e)
        print(colored(f"  [ERROR] Failed to save data: {e}", Colors.RED))
        cursor._connection.rollback()
        return success


def end_session(cursor, session_id):
    """
    Update session updated_at timestamp when program ends.
    """
    try:
        cursor.execute("""
            UPDATE Session SET updated_at = NOW() WHERE id = %s
        """, (session_id,))
        cursor._connection.commit()
        logger.info("[SESSION] Ended session %d", session_id)
        print(colored(f"[SESSION] Session {session_id} ended", Colors.CYAN))
    except mysql.connector.Error as e:
        logger.error("[SESSION] Failed to end session: %s", e)


if __name__ == "__main__":
    # Configuration
    MODBUS_IP = "192.168.60.4"
    MODBUS_PORT = 502
    
    SQL_IP = "192.168.70.4"
    SQL_PORT = 3306
    SQL_USER = "data_logger"
    SQL_PASSWORD = "*F645AE27B7F3EE07F39C5FC96B0B2410CB1D2BEF"
    DATABASE = "production"
    
    TABLE_CONFIGS = [
        {
            "table": "SlittingAndNotchingPLCHistory",
            "prefix": "slitnotch"
        }
    ]
    
    interval = 0.2  # seconds between readings
    
    # --- Main initialization ---
    print(colored("\n  Initializing PLC Data Logger...", Colors.BRIGHT_WHITE))
    print(colored(f"  [MODBUS] Target: {MODBUS_IP}:{MODBUS_PORT}", Colors.CYAN))
    print(colored(f"  [MYSQL]  Target: {SQL_IP}:{SQL_PORT}/{DATABASE}", Colors.CYAN))
    print(colored(f"  [CONFIG] Interval: {interval}s  |  Tables: {len(TABLE_CONFIGS)}", Colors.CYAN))
    print()
    
    # --- Connect to MySQL ---
    try:
        db = mysql.connector.connect(
            host=SQL_IP,
            port=SQL_PORT,
            user=SQL_USER,
            password=SQL_PASSWORD,
            database=DATABASE
        )
        cursor = db.cursor()
        print(colored("[MYSQL] Connected to database", Colors.BRIGHT_GREEN))
    except mysql.connector.Error as e:
        print(colored(f"[FATAL] MySQL connection failed: {e}", Colors.RED))
        sys.exit(1)
    
    # --- Create ONE session for this program run ---
    # The session uses the Modbus IP to resolve the interface name
    session_id = create_session(cursor, MODBUS_IP, SQL_IP, SQL_PORT, TABLE_CONFIGS[0]["prefix"])
    if session_id is None:
        print(colored("[FATAL] Could not create session. Exiting.", Colors.RED))
        cursor.close()
        db.close()
        sys.exit(1)
    
    print()
    
    # --- Main logging loop ---
    try:
        while True:
            for config in TABLE_CONFIGS:
                log_data(
                    modbus_ip=MODBUS_IP,
                    modbus_port=MODBUS_PORT,
                    cursor=cursor,
                    history_sql_table=config["table"],
                    plc_variable_id_prefix=config["prefix"],
                    session_id=session_id
                )
            
            time.sleep(interval)
            
    except KeyboardInterrupt:
        print("\n")
        print(colored("  [SHUTDOWN] Logging stopped by user", Colors.YELLOW))
        print()
    except Exception as e:
        logger.error("[FATAL] %s", e)
        print(colored(f"\n  [FATAL] Unexpected error: {e}", Colors.RED))
        print()
    finally:
        # --- End session and clean up ---
        end_session(cursor, session_id)
        cursor.close()
        db.close()
        print(colored("[MYSQL] Connection closed", Colors.DIM))
        print()