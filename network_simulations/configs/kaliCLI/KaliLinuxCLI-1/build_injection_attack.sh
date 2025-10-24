#!/bin/bash

# Update package lists
apt update

# Install g++ if not already installed
apt install -y g++

# Define the script and output locations
script="/home/Attacks/DDoS/lib/dos.cpp"
output="/home/Attacks/DDoS/dos"

# Compile the C++ code
g++ -o "$output" "$script" -lpthread