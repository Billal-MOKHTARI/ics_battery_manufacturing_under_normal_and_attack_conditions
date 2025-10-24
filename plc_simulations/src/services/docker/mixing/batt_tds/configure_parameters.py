import sys
import os
from pymodbus.client import ModbusTcpClient
from colorama import Fore, Style



# Add parent directory to system path
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '../../../..')))
from utils import io

# Load the configurations
configs = io.read_json("configs/vars.json")

# Import the physical address table
physical_addr = configs["physical_addressing"]
sensors = configs["sensors"]
actuators = configs["actuators"]

# Get user input for PLC connection
print(Fore.YELLOW + "📡 Enter the IP address of the PLC: " + Fore.RESET)
ip_address = input().strip()

print(Fore.YELLOW + "🔌 Enter the port number: " + Fore.RESET)
try:
    port = int(input().strip())
except ValueError:
    print(Fore.RED + "❌ Invalid port number! Please enter a valid integer." + Style.RESET_ALL)
    sys.exit(1)  # Exit the script if an invalid port is entered

try:
    # Establish Modbus TCP connection
    client = ModbusTcpClient(ip_address, port=port)
    if not client.connect():
        print(Fore.RED + "❌ Failed to connect to PLC!" + Style.RESET_ALL)
        sys.exit(1)

    print(Fore.GREEN + "✅ Connection successfully established!" + Style.RESET_ALL)

    # Get default values from PLC
    default_timer_gauge_1 = client.read_holding_registers(address=physical_addr["timer_gauge_1"], count=1)
    default_timer_gauge_2 = client.read_holding_registers(address=physical_addr["timer_gauge_2"], count=1)
    default_n_mixing_times = client.read_holding_registers(address=physical_addr["PV"], count=1)

    # Extract actual values safely
    default_timer_gauge_1 = default_timer_gauge_1.registers[0] if default_timer_gauge_1 else "N/A"
    default_timer_gauge_2 = default_timer_gauge_2.registers[0] if default_timer_gauge_2 else "N/A"
    default_n_mixing_times = default_n_mixing_times.registers[0] if default_n_mixing_times else "N/A"

    while True:
        print("1. Set the operating time for Engine M1")
        print("2. Set the operating time for Engine M2")
        print("3. Set the number of mixing cycles")
        print("4. Exit")
        print(Fore.YELLOW + "🔧 Select an option: " + Fore.RESET)
        option = input().strip()
        io.clear_terminal()

        if option == "1":
            print(Fore.YELLOW + f"⏳ Enter the operating time for Engine M1 (seconds) [Default: {default_timer_gauge_1}]: " + Fore.RESET)
            timer_gauge_1 = input().strip()
            client.write_register(address=physical_addr["timer_gauge_1"], value=int(timer_gauge_1))
            print(Fore.GREEN + "✅ Engine M1 operating time successfully updated!" + Style.RESET_ALL)
        elif option == "2":
            print(Fore.YELLOW + f"⏳ Enter the operating time for Engine M2 (seconds) [Default: {default_timer_gauge_2}]: " + Fore.RESET)
            timer_gauge_2 = input().strip()
            client.write_register(address=physical_addr["timer_gauge_2"], value=int(timer_gauge_2))
            print(Fore.GREEN + "✅ Engine M2 operating time successfully updated!" + Style.RESET_ALL)
        elif option == "3":
            print(Fore.YELLOW + f"🔄 Enter the number of mixing cycles [Default: {default_n_mixing_times}]: " + Fore.RESET)
            n_mixing_times = input().strip()
            client.write_register(address=physical_addr["PV"], value=int(n_mixing_times))
            print(Fore.GREEN + "✅ Number of mixing cycles successfully updated!" + Style.RESET_ALL)
        elif option == "4":
            break
except Exception as e:
    print(Fore.RED + f"⚠️ Connection error: {e}" + Style.RESET_ALL)
finally:
    client.close()
    print(Fore.YELLOW + "🔌 Connection closed." + Style.RESET_ALL)
