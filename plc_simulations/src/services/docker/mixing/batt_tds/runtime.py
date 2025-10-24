import time
import sys
import os
from pymodbus.client import ModbusTcpClient
from colorama import Fore, Style, init

# Initialize colorama
init(autoreset=True)

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '../../../..')))
from utils import io

# Load the configurations
configs = io.read_json("configs/vars.json")

# Import the physical address table
physical_addr = configs["physical_addressing"]
sensors = configs["sensors"]
actuators = configs["actuators"]

while True:
    print(Fore.YELLOW + "Enter the IP address of the PLC: ", Fore.RESET)
    ip_address = input().strip()

    print(Fore.YELLOW + "Enter the port number: ", Fore.RESET)
    try:
        port = int(input().strip())
    except ValueError:
        print(Fore.RED + "Invalid port number! Please enter a valid integer." + Style.RESET_ALL)
        continue

    try: 
        client = ModbusTcpClient(ip_address, port=port)
        connection_status = client.connect()

        if not connection_status:
            print(Fore.RED + "Failed to connect to the PLC. Check the IP address and port." + Style.RESET_ALL)
            continue

        print(Fore.GREEN + "✅ Connection successfully established!" + Style.RESET_ALL)
        
        while True:
            for key in physical_addr.keys():
                try:
                    f_value = client.read_coils(address=physical_addr["f"], count=1).bits[0]
                    l_value = client.read_coils(address=physical_addr["l"], count=1).bits[0]
                    e2_value = client.read_coils(address=physical_addr["e2"], count=1).bits[0]
                    d2_value = client.read_coils(address=physical_addr["d2"], count=1).bits[0]

                    if f_value and l_value:
                        print(Fore.YELLOW + "Turning off 'f' and 'l'..." + Style.RESET_ALL)
                        time.sleep(1)
                        client.write_coil(physical_addr["f"], False)
                        client.write_coil(physical_addr["l"], False)

                    if d2_value and e2_value:
                        print(Fore.YELLOW + "Turning off 'd2' and 'e2'..." + Style.RESET_ALL)
                        time.sleep(1)
                        client.write_coil(physical_addr["d2"], False)
                        client.write_coil(physical_addr["e2"], False)

                    if key in sensors and key not in ["f", "l", "e2", "d2"]:
                        value = client.read_coils(address=physical_addr[key], count=1).bits[0]
                        if value:
                            print(Fore.GREEN + f"✅ Sensor {key} triggered. Turning it off..." + Style.RESET_ALL)
                            time.sleep(1)
                            client.write_coil(physical_addr[key], False)
                
                except Exception as e:
                    print(Fore.RED + f"Error reading/writing {key}: {e}" + Style.RESET_ALL)

    except Exception as e:
        print(Fore.RED + f"⚠️ Connection error: {e}" + Style.RESET_ALL)
