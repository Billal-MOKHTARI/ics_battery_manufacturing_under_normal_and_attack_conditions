import mysql.connector
import time
import sys
import os
from datetime import datetime
from plc_simulations.src.utils import system
from pymodbus.client import ModbusTcpClient
from colorama import Fore, Style, init
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '../../../..')))
from utils import io, system
from system import mask_to_cidr, exec_sql


# Get network info
linux_ip, linux_mask = system.get_linux_ip_and_mask()
cidr = mask_to_cidr(linux_mask) if linux_mask else None

# Initialize colorama
init(autoreset=True)

# Clear terminal
io.clear_terminal()

# Load configurations
try:
    configs = io.read_json("configs/vars.json")
    physical_addr = configs["physical_addressing"]
    holding_registers = ["PV", "CV", "time_1", "time_2", "timer_gauge_1", "timer_gauge_2", "CU"]
except Exception as e:
    print(Fore.RED + f"Error loading configurations: {str(e)}" + Style.RESET_ALL)
    sys.exit(1)  # Exit if configuration loading fails

while True:
    try:
        # Modbus connection
        print(Fore.YELLOW + "Enter the PLC IP address: ", Fore.RESET)
        plc_ip_address = input().strip()

        print(Fore.YELLOW + "Enter the port number of the Modbus protocol (e.g., 502): ", Fore.RESET)
        plc_port = input().strip()
        if not plc_port.isdigit():
            print(Fore.RED + "Invalid port number! Please enter a valid integer." + Style.RESET_ALL)
            continue
        plc_port = int(plc_port)

        # Initialize Modbus Client
        client = ModbusTcpClient(plc_ip_address, port=plc_port)
        if not client.connect():
            print(Fore.RED + "❌ Connection to PLC failed!" + Style.RESET_ALL)
            continue

        print(Fore.GREEN + "✅ Connection to PLC established!" + Style.RESET_ALL)

        # MySQL connection
        print(Fore.YELLOW + "Enter the MySQL IP address: ", Fore.RESET)
        mysql_ip_address = input().strip()

        print(Fore.YELLOW + "Enter the port number of the MySQL protocol (e.g., 3306): ", Fore.RESET)
        mysql_port = input().strip()
        if not mysql_port.isdigit():
            print(Fore.RED + "Invalid port number! Please enter a valid integer." + Style.RESET_ALL)
            continue
        mysql_port = int(mysql_port)

        print(Fore.YELLOW + "Enter the MySQL username: ", Fore.RESET)
        mysql_username = input().strip()
        print(Fore.YELLOW + "Enter the MySQL password: ", Fore.RESET)
        mysql_password = input().strip()
        database_name = "production"


        system = mysql.connector.connect(
            host=mysql_ip_address,
            user=mysql_username,
            password=mysql_password,
            database=database_name,
            port=mysql_port
        )

        if system.is_connected():
            print(Fore.GREEN + "✅ Connection to MySQL established!" + Style.RESET_ALL)
        else:
            print(Fore.RED + "❌ Connection to MySQL failed!" + Style.RESET_ALL)
            continue

        
        # Time interval
        print(Fore.YELLOW + "Enter the logging time interval (in seconds): ", Fore.RESET)
        t = input().strip()

        try:
            t = float(t)
            if t <= 0:
                raise ValueError("Time interval must be a positive number.")
        except ValueError as e:
            print(Fore.RED + f"Invalid time interval: {e}" + Style.RESET_ALL)
            continue


        cur = system.cursor(buffered=True)

        session = f"""
        INSERT INTO Session (
            machine, 
            interface, 
            port, 

            src_ipv4, 
            src_mask, 

            dest_ipv4,
            dest_port,
            
            created_at, 
            updated_at
        )
        VALUES (
            'Batt-TDS Mixing Machine',
            (SELECT name FROM Interface WHERE ipv4 = '{plc_ip_address}'),
            {plc_port},
            '{linux_ip}',
            {cidr},  -- Now using numeric CIDR instead of string mask
            '{mysql_ip_address}',
            {mysql_port},

            NOW(),
            NOW()
        );
        """

        sql_session_id = f"SELECT LAST_INSERT_ID();"
        exec_sql(session, system, cur)
        session_id = exec_sql(sql_session_id, system, cur)[0][0]

        vars = {}
        # Main monitoring loop
        while True:
            timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

            for key, address in physical_addr.items():
                try:
                    # Determine read type
                    response = client.read_holding_registers(address, count=1) if key in holding_registers else client.read_coils(address, count=1)

                    if not response.isError():
                        value = response.registers[0] if key in holding_registers else response.bits[0]
                        print(f"{Fore.CYAN}[{timestamp}] {key}: {Fore.GREEN}{value}{Style.RESET_ALL}")
                        vars[key] = int(value)
                    else:
                        print(Fore.RED + f"❌ Error reading {key}" + Style.RESET_ALL)

                except Exception as e:
                    print(Fore.RED + f"❌ Error reading {key}: {str(e)}" + Style.RESET_ALL)
            add_data = f"""
            INSERT INTO MixingPLCHistory (session, m_id, a_id, b_id, d1_id, d2_id, e1_id, e2_id, f_id, g_id, l_id, EA_id, EB_id, ED_id, EE_id, EF_id, EG_id, M1_id, M2_id, X0_id, X1_id, X2_id, X3_id, X4_id, X5_id, X6_id, X7_id, X8_id, T0_id, T1_id, T2_id, T3_id, T4_id, T5_id, T6_id, T7_id, T8_id, PV_id, CV_id, CU_id, time_1_id, time_2_id, timer_gauge_1_id, timer_gauge_2_id, m, a, b, d1, d2, e1, e2, f, g, l, EA, EB, ED, EE, EF, EG, M1, M2, X0, X1, X2, X3, X4, X5, X6, X7, X8, T0, T1, T2, T3, T4, T5, T6, T7, T8, PV, CV, CU, time_1, time_2, timer_gauge_1, timer_gauge_2, created_at, updated_at)
            VALUES (
                {session_id},
                "batttdsmixm",
                "batttdsmixa",
                "batttdsmixb",
                "batttdsmixd1",
                "batttdsmixd2",
                "batttdsmixe1",
                "batttdsmixe2",
                "batttdsmixf",
                "batttdsmixg",
                "batttdsmixl",
                "batttdsmixEA",
                "batttdsmixEB",
                "batttdsmixED",
                "batttdsmixEE",
                "batttdsmixEF",
                "batttdsmixEG",
                "batttdsmixM1",
                "batttdsmixM2",
                "batttdsmixX0",
                "batttdsmixX1",
                "batttdsmixX2",
                "batttdsmixX3",
                "batttdsmixX4",
                "batttdsmixX5",
                "batttdsmixX6",
                "batttdsmixX7",
                "batttdsmixX8",
                "batttdsmixT0",
                "batttdsmixT1",
                "batttdsmixT2",
                "batttdsmixT3",
                "batttdsmixT4",
                "batttdsmixT5",
                "batttdsmixT6",
                "batttdsmixT7",
                "batttdsmixT8",
                "batttdsmixPV",
                "batttdsmixCV",
                "batttdsmixCU",
                "batttdsmixtime_1",
                "batttdsmixtime_2",
                "batttdsmixtimer_gauge_1",
                "batttdsmixtimer_gauge_2",
                {vars["m"]},
                {vars["a"]},
                {vars["b"]},
                {vars["d1"]},
                {vars["d2"]},
                {vars["e1"]},
                {vars["e2"]},
                {vars["f"]},
                {vars["g"]},
                {vars["l"]},
                {vars["EA"]},
                {vars["EB"]},
                {vars["ED"]},
                {vars["EE"]},
                {vars["EF"]},
                {vars["EG"]},
                {vars["M1"]},
                {vars["M2"]},
                {vars["X0"]},
                {vars["X1"]},
                {vars["X2"]},
                {vars["X3"]},
                {vars["X4"]},
                {vars["X5"]},
                {vars["X6"]},
                {vars["X7"]},
                {vars["X8"]},
                {vars["T0"]},
                {vars["T1"]},
                {vars["T2"]},
                {vars["T3"]},
                {vars["T4"]},
                {vars["T5"]},
                {vars["T6"]},
                {vars["T7"]},
                {vars["T8"]},
                {vars["PV"]},
                {vars["CV"]},
                {vars["CU"]},
                {vars["time_1"]},
                {vars["time_2"]},
                {vars["timer_gauge_1"]},
                {vars["timer_gauge_2"]},
                NOW(),
                NOW()
            );
            """
            exec_sql(add_data, system, cur)

            time.sleep(t)  # Use the user-defined logging interval


    finally:
        client.close()
        system.close()
        print(Fore.YELLOW + "🔄 Connection closed.\n" + Style.RESET_ALL)
