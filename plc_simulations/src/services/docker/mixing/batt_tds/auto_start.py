import time
import sys
import os
import time
from pymodbus.client import ModbusTcpClient
from colorama import Fore, Style

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '../../../..')))
from utils import io

# Load the configurations
configs = io.read_json("configs/vars.json")


# Import the physical address table
physical_addr = configs["physical_addressing"]

sensors = configs["sensors"]
actuators = configs["actuators"]

print(Fore.YELLOW + "Enter the IP address of the PLC: ", Fore.RESET)
ip_address = input().strip()

print(Fore.YELLOW + "Enter the port number: ", Fore.RESET)
try:
    port = int(input().strip())
except ValueError:
    print(Fore.RED + "Invalid port number! Please enter a valid integer." + Style.RESET_ALL)

print(Fore.YELLOW + "Enter the time interval between events: ", Fore.RESET)
t = int(input().strip())

try: 
    client = ModbusTcpClient(ip_address, port=port)
    connection_status = client.connect()

    print(Fore.GREEN + "✅ Connection successfully established!" + Style.RESET_ALL)
    sequence = [["m"], ["d1", "e1"], ["a", "b"], ["d2", "e2"]]

    while True:
        PV = client.read_holding_registers(address=physical_addr["PV"], count=1).registers[0]
        for sensor in sequence:
            for e in sensor:
                client.write_coil(physical_addr[e], True)
            time.sleep(t)

        for i in range(PV):
            time.sleep(client.read_holding_registers(address=physical_addr["timer_gauge_1"], count=1).registers[0])
            client.write_coil(physical_addr["f"], True)
            client.write_coil(physical_addr["l"], True)
            time.sleep(client.read_holding_registers(address=physical_addr["timer_gauge_2"], count=1).registers[0])
            client.write_coil(physical_addr["g"], True)

except Exception as e:
    print(Fore.RED + f"⚠️ Connection error: {e}" + Style.RESET_ALL)