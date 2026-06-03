#!/bin/bash

# Delete CSV files in current directory
find . -maxdepth 1 -type f -name "*.csv" -delete

# Delete CSV files inside MF/
find MF -maxdepth 1 -type f -name "*.csv" -delete

# Delete JSON files inside MF/fuzzing_sessions/
find MF/fuzzing_sessions -type f -name "*.json" -delete

echo "Cleanup completed."