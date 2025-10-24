# ICS battery manufacturing under normal and attack conditions

This repository describes the steps to be followed by a user to reproduce the network topology on which attack experiments will be tested on and the data will be collected from. We got three main folders:

- **network_simulations**: It contains the complete configuration of each node of the network.
- **plc_simulations**: It contains the configurations to set up the production and SCADA environment. Especially, how the PLC program to run the ModBus protocol.
- **attacks**: Contains the CLI programs to launch code injection and DoS attacks.