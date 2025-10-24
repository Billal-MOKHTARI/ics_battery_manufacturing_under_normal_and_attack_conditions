import time
import sys
import os
from datetime import datetime
from pymodbus.client import ModbusTcpClient
from colorama import Fore, Style, init
from comet_ml import Experiment, ExistingExperiment

# Initialize colorama
init(autoreset=True)

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '../../../..')))
from utils import io, file_manager
from file_manager import get_experiment_key, save_experiment_key

# Clear terminal
io.clear_terminal()

try:
    # Load configurations
    configs = io.read_json("configs/vars.json")
    physical_addr = configs["physical_addressing"]
    sensors = configs["sensors"]
    actuators = configs["actuators"]
    holding_registers = ["PV", "CV", "time_1", "time_2", "timer_gauge_1", "timer_gauge_2"]
except Exception as e:
    print(Fore.RED + f"Error loading configurations: {str(e)}" + Style.RESET_ALL)
    sys.exit(1)  # Exit if configuration loading fails

EXPERIMENT_KEY_FILE = "secrets/comet_experiment.key"


while True:
    try:
        print(Fore.YELLOW + "Enter the PLC IP address: ", Fore.RESET)
        ip_address = input().strip()

        print(Fore.YELLOW + "Enter the port number (e.g., 502): ", Fore.RESET)
        port = input().strip()
        if not port.isdigit():
            print(Fore.RED + "Invalid port number! Please enter a valid integer." + Style.RESET_ALL)
            continue
        port = int(port)

        # Comet ML Inputs
        print(Fore.YELLOW + "Enter the Comet API key: ", Fore.RESET)
        api_key = input().strip()
        print(Fore.YELLOW + "Enter the Comet workspace: ", Fore.RESET)
        workspace = input().strip()
        print(Fore.YELLOW + "Enter the Comet project name: ", Fore.RESET)
        project_name = input().strip()

        print(Fore.YELLOW + "Enter the logging time interval (in seconds): ", Fore.RESET)
        t = input().strip()
        try:
            t = float(t)
            if t <= 0:
                raise ValueError("Time interval must be a positive number.")
        except ValueError as e:
            print(Fore.RED + f"Invalid time interval: {e}" + Style.RESET_ALL)
            continue

        # Initialize Comet Experiment
        try:
            experiment = Experiment(
                api_key=api_key,
                project_name=project_name,
                workspace=workspace,
                auto_metric_logging=False,
                auto_param_logging=False,
                log_code=False
            )
        except Exception as e:
            print(Fore.RED + f"Error initializing Comet experiment: {str(e)}" + Style.RESET_ALL)
            continue

        # Initialize Modbus Client
        client = ModbusTcpClient(ip_address, port=port)
        if not client.connect():
            print(Fore.RED + "❌ Connection to PLC failed!" + Style.RESET_ALL)
            continue

        print(Fore.GREEN + "✅ Connection to PLC established!" + Style.RESET_ALL)

        # Resume previous experiment if available
        experiment_key = get_experiment_key(filename=EXPERIMENT_KEY_FILE)
        if experiment_key:
            try:
                experiment = ExistingExperiment(
                    api_key=api_key,
                    project_name=project_name,
                    workspace=workspace,
                    previous_experiment=experiment_key,
                    auto_metric_logging=False,
                    log_code=False
                )
            except Exception as e:
                print(Fore.RED + f"Error resuming Comet experiment: {str(e)}" + Style.RESET_ALL)
                continue
        else:
            experiment.set_name(f"PLC_{ip_address}_{datetime.now().strftime('%Y%m%d')}")
            save_experiment_key(experiment.get_key(), filename=EXPERIMENT_KEY_FILE)

        # Log connection parameters
        experiment.log_parameters({
            "ip_address": ip_address,
            "port": port,
            "plc_type": "Modbus TCP"
        })

        # Main monitoring loop
        while True:
            timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            for key, address in physical_addr.items():
                try:
                    # Determine read type
                    response = client.read_holding_registers(address, count=1) if key in holding_registers else client.read_coils(address, count=1)

                    if not response.isError():
                        value = response.registers[0] if key in holding_registers else response.bits[0]

                        # Determine device type
                        device_type = "sensor" if key in sensors else "actuator"

                        # Log metadata separately
                        device_info = {
                            "device_type": device_type,
                            "register_type": "holding_register" if key in holding_registers else "coil",
                            "address": address,
                            "ip": ip_address,
                            "port": port,
                            "timestamp": timestamp
                        }
                        experiment.log_other(f"{key}_info", device_info)

                        # Log the metric with basic parameters
                        experiment.log_metric(name=key, value=value, step=int(datetime.now().timestamp()))

                        print(f"{Fore.CYAN}[{timestamp}] {key}: {Fore.GREEN}{value}{Style.RESET_ALL}")
                    else:
                        print(Fore.RED + f"❌ Error reading {key}" + Style.RESET_ALL)

                except Exception as e:
                    print(Fore.RED + f"❌ Error reading {key}: {str(e)}" + Style.RESET_ALL)

            time.sleep(t)  # Use the user-defined logging interval

    except KeyboardInterrupt:
        print(Fore.YELLOW + "\n🔴 Program interrupted by user. Exiting..." + Style.RESET_ALL)
        break

    except Exception as e:
        print(Fore.RED + f"❌ Critical error: {str(e)}" + Style.RESET_ALL)
        experiment.log_other("Critical Error", str(e))

    finally:
        client.close()
        print(Fore.YELLOW + "🔄 Connection closed. Restarting...\n" + Style.RESET_ALL)
