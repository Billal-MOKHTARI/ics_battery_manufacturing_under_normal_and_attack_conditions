import sys
import os
import time
from pymodbus.client import ModbusTcpClient
from colorama import Fore, Style

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '../../../..')))
from utils import io, system

# Load the configurations
configs = io.read_json("configs/vars.json")
connection = configs["connection"]

# Establish connection to the slave device
ip_address = connection["ip_address"]
port = connection["port"]

# Import the physical address table
physical_addr = configs["physical_addressing"]

sensors = configs["sensors"]
actuators = configs["actuators"]

try: 
    client = ModbusTcpClient(ip_address, port=port)
    connection_status = client.connect()
    print(Fore.GREEN + "Connection successfully established" + Style.RESET_ALL)

except:
    print(Fore.RED+"The IP address or the port is incorrect. Please check the configurations."+Style.RESET_ALL)
