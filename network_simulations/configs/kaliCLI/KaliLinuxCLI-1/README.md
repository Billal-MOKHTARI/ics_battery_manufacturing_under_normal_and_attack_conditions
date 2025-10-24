This code was used to inject modbus commands to the water heater experiment.

To compile: g++ injection_attack.cpp -o injection_attack -pthread
To run: ./injection_attack -h [host ip] -f [frequency of the messages]