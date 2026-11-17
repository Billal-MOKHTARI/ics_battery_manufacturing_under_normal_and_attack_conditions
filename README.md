# ICS Battery Manufacturing Simulation and Attack Framework

This repository contains the code used to build and operate the CyBattICS testbed, an industrial control system (ICS) that emulates three stages of lithium-ion battery electrode manufacturing (mixing, coating, and slitting and notching). It provides the network configuration of the testbed, the PLC programs and supervisory services, the attack tools used to generate the attack scenarios, and the script that derives the processed operational data released with the dataset.

The dataset itself is available on Zenodo at [10.5281/zenodo.22086671](https://doi.org/10.5281/zenodo.22086671).

## Repository structure

| Folder | Content |
| --- | --- |
| `network_simulations/` | Configuration of the emulated network, including Cisco router and switch configurations, Open vSwitch and ONOS settings, the ELK stack, the MySQL server, OpenPLC, SCADA-LTS, and the attacker node |
| `plc_simulations/` | PLC programs of the three processes, Modbus services, data loggers and monitors, a graphical simulator of the mixing machine, and Docker build helpers |
| `attacks/` | Attack modules (code injection, bias injection, coil manipulation, holding register manipulation, desynchronization, PLC reset, denial of service), the grammar-based Modbus fuzzer, and the attack orchestration scripts |
| `processing/` | Script that builds `historical_data_processed.csv` from the raw operational records and the attack logs of the dataset |

### Network simulation (`network_simulations/`)

This folder documents the network topology and the configuration of each node. It is a collection of configuration files and notes rather than a single deployable service.

The complete GNS3 project, including the virtual machine and appliance images of all nodes, will be made publicly available. Its size (approximately 400 GB) exceeds the limits of standard code and data repositories, and we are currently evaluating suitable hosting options. Until then, the configuration files in this folder document the configuration of the main nodes of the topology.

### PLC simulation and services (`plc_simulations/`)

| Path | Description |
| --- | --- |
| `plc_programs/` | IEC 61131-3 programs (Beremiz/OpenPLC projects) of the mixing, coating, and slitting-and-notching PLCs |
| `src/services/system/mixing/batt_tds/` | Modbus client services built around `configs/vars.json` |
| `src/services/docker/` | Container entry points for the PLC runtime, auto-start, parameter configuration, data loggers, and data monitor |
| `src/utils/` | Utility functions for file handling, JSON loading, and terminal management |
| `scripts/mixing/batt_tds/` | Docker image build scripts for the mixing services |
| `jobs/` | Scripts to build and load Docker images |
| `configs/vars.json` | Static PLC address mapping |

### Attack tools (`attacks/`)

| Path | Description |
| --- | --- |
| `code_injection/` | Modbus code injection tool with command-line and interactive wrappers |
| `bias_injector/` | Process variable bias injection attack (PVBIA) |
| `coil_manipulation/` | Coil state manipulation attack |
| `holding_register_manipulation/` | Holding register false data injection (HR-FDI) |
| `desynchronizer/` | Desynchronization attack |
| `plc_reset/` | Deadlock attack that forces all coils to OFF except coil 0 |
| `ddos/` | Denial-of-service tool written in C++, with command-line and interactive wrappers |
| `MF/` | Grammar-based Modbus fuzzer with its own dependencies and command-line interface |
| `attack_scenarios/` | Orchestration scripts that run randomized attack sequences against the coating and slitting-and-notching PLCs |
| `cleanup.sh` | Removes the log files and fuzzing reports produced by the attacks |

### Data processing (`processing/`)

| File | Description |
| --- | --- |
| `process.py` | Builds the processed operational data (column selection, expected signal values, attack labels) |
| `reference_signals.py` | Reference trajectories of every SFC step, as defined in the PLC programs |
| `requirements.txt` | Python dependencies of the processing script |

## Requirements

- Linux or macOS with Python 3 and `pip`
- `pymodbus` and `colorama` for the attack modules
- `docker` and `docker buildx` to build the container images
- `nmap` for the wrapper scripts that scan a subnet
- `pandas` and `numpy` for the processing script

## Installation

From the repository root, install the dependencies of the components you intend to use.

```bash
python3 -m pip install --upgrade pip
python3 -m pip install pymodbus colorama
python3 -m pip install -r attacks/MF/requirements.txt
python3 -m pip install -r processing/requirements.txt
```

The data loggers and the data monitor require additional packages.

```bash
python3 -m pip install mysql-connector-python netifaces comet_ml
sudo apt install nmap
```

## Building the PLC services

The Docker images of the mixing services are built with the following scripts.

```bash
cd plc_simulations
./scripts/mixing/batt_tds/build_simulator.sh
./scripts/mixing/batt_tds/build_runtime.sh
./scripts/mixing/batt_tds/build_auto_start.sh
./scripts/mixing/batt_tds/build_data_logger.sh
./scripts/mixing/batt_tds/build_data_monitor.sh
./scripts/mixing/batt_tds/configure_parameters.sh
```

The script `jobs/load_docker_images.sh` loads saved images from `plc_simulations/docker/images/`. These image archives are not distributed with the repository, so images must be built locally with the scripts above or supplied separately.

## Main entry points

### PLC runtime and services

| File | Description |
| --- | --- |
| `plc_simulations/main.py` | Minimal Modbus client example that writes coil 0 of a PLC |
| `plc_simulations/src/services/system/mixing/batt_tds/runtime.py` | Steady-state PLC runtime that updates coils from the configured physical addresses |
| `plc_simulations/src/services/docker/mixing/batt_tds/runtime.py` | Container entry point for the interactive PLC runtime loop |
| `plc_simulations/src/services/docker/mixing/batt_tds/data_logger.py` | Records the Modbus values of the mixing PLC in the MySQL database |
| `plc_simulations/src/services/docker/coating/data_logger.py` | Records the Modbus values of the coating PLC in the MySQL database |
| `plc_simulations/src/services/docker/slitting_notching/data_logger.py` | Records the Modbus values of the slitting-and-notching PLC in the MySQL database |
| `plc_simulations/src/services/docker/mixing/batt_tds/data_monitor.py` | Streams PLC sensor values to Comet ML |
| `plc_simulations/src/services/docker/mixing/batt_tds/configure_parameters.py` | Updates PLC timers and cycle counts through Modbus |
| `plc_simulations/src/services/docker/mixing/batt_tds/auto_start.py` | Emulates the automatic start of the mixing process by toggling coils over time |

### Attack modules

| File | Description |
| --- | --- |
| `attacks/code_injection/code_injection.py` | Code injection attack on coils and holding registers |
| `attacks/code_injection/code_injection_cli.sh` | Discovers Modbus hosts on a subnet and runs the code injection attack against each of them |
| `attacks/code_injection/code_injection_interactive.sh` | Interactive wrapper for the code injection attack |
| `attacks/attack_scenarios/as_coating.py` | Randomized attack schedule against the coating PLC |
| `attacks/attack_scenarios/as_slitting_notching.py` | Randomized attack schedule against the slitting-and-notching PLC |
| `attacks/MF/modFuzzer.py` | Grammar-based Modbus fuzzing tool |

## Example workflows

### Code injection attack

```bash
cd attacks/code_injection
python3 code_injection.py \
  --ip 192.168.60.3 \
  --port 502 \
  --mode down \
  --n_coils 100 \
  --n_holding_registers 100 \
  --hr_value 0 \
  --time 60
```

### Attack orchestration

```bash
python3 attacks/attack_scenarios/as_coating.py
```

### Grammar-based Modbus fuzzing

```bash
cd attacks/MF
python3 modFuzzer.py --grammar 192.168.60.3 --duration 60 --log_file sessions.csv --output_dir ./reports
```

### Data logger

```bash
python3 plc_simulations/src/services/docker/coating/data_logger.py
```

### Processing the dataset

The processing script regenerates `historical_data_processed.csv` for the coating and slitting-and-notching processes. `<dataset folder>` is the folder of the downloaded dataset that contains `coating_data/` and `slitting_notching_data/`.

```bash
cd processing
python3 process.py --process coating --data-root <dataset folder> --out coating_processed.csv
python3 process.py --process slitting_notching --data-root <dataset folder> --out slitting_notching_processed.csv
```

The script does not overwrite an existing output file unless `--overwrite` is given. It performs three operations.

1. **Column selection.** It keeps `id`, `signalX*`, `outX*`, and `created_at`, renames them `obs_signal_value(X*)`, `status(X*)`, and `timestamp`, and renumbers `id` from 0.
2. **Expected signal values.** While step X<sub>i</sub> is active, `exp_signal_value(X_i)` is the reference trajectory of the step evaluated at the time elapsed since its activation, and it is 0 otherwise. The activation time is the timestamp of the last transition of `status(X_i)` from 0 to 1, and the reference trajectories are defined in `reference_signals.py`.
3. **Attack labels.** `attack_type` takes the type of the attack window that contains the record timestamp, bounds included, and `normal` otherwise. Standard attack logs provide `start_time` and `end_time`. The grammar-fuzzing log provides the end of each session and its duration, so the session window is `[timestamp - duration_sec, timestamp]`. The 1-s PLC reset executed after each fuzzing session is recorded in `log_deadlock.csv` and is labelled as part of that fuzzing session.

The expected values should be interpreted with the following limitations in mind.

- Record timestamps have a resolution of one second, so elapsed times are whole seconds.
- The PLC evaluates the reference trajectories on an internal clock that advances by 0.1 at each 20-ms cycle, whereas the script uses wall-clock seconds. Transients therefore do not match exactly, while constant segments are unaffected.
- When the signal generator of an active step has terminated (`termX_i` equal to 1 in the raw file), the PLC signal falls to its noise terms while the expected value keeps following the reference trajectory.
- For step X9 of the slitting-and-notching PLC, the PLC program uses 3.0 instead of π, whereas `reference_signals.py` uses π.

The script was tested with Python 3.13, pandas 3.0.5, and numpy 2.5.3. Its output is identical, byte for byte, to the processed files of the revised dataset version.

## Configuration

Most runtime settings are defined in the code and in JSON files rather than in environment variables. Network addresses must be adapted before the tools are used in another environment.

| Location | Settings |
| --- | --- |
| `plc_simulations/configs/vars.json` | `connection.ip_address`, `connection.port`, `physical_addressing`, and the sensor and actuator address maps |
| `plc_simulations/src/services/docker/coating/data_logger.py` | Modbus target `192.168.60.3:502`, MySQL server `192.168.70.4:3306`, database `production`, user `data_logger` |
| `attacks/bias_injector/bi_coating.py` | Modbus target `192.168.60.3:502` |
| `attacks/attack_scenarios/as_coating.py` | Coating PLC at `192.168.60.3` |
| `attacks/attack_scenarios/as_slitting_notching.py` | Slitting-and-notching PLC at `192.168.60.4` |

No environment variable is read by the Python attack or PLC service code. The script `network_simulations/configs/ubuntu_toolbox/config.sh` exports `JAVA_HOME`.

### Required and optional components

| Component | Status |
| --- | --- |
| Python 3, `pymodbus`, and `colorama` | Required for the attack modules |
| A reachable Modbus/TCP target, typically `192.168.60.3` or `192.168.60.4` | Required for the attack modules |
| `nmap` | Required for the wrapper scripts in `attacks/code_injection/` |
| `pandas` and `numpy` | Required for the processing script |
| `mysql-connector-python` and `netifaces` | Optional, used by the data loggers |
| `comet_ml` | Optional, used by the data monitor |
| Docker and Docker Buildx | Optional, used to build the container images |
| Dependencies in `attacks/MF/requirements.txt` | Optional, used by the fuzzer |

## Troubleshooting

| Symptom | Resolution |
| --- | --- |
| `ModuleNotFoundError: No module named 'pymodbus'` | Install the package with `pip3 install pymodbus` |
| `python3: command not found` | Install Python 3 and make sure it is on the `PATH` |
| The wrapper scripts do not discover any PLC | Install `nmap`, check the target subnet and port, and verify that the PLC is reachable from the host |
| `Permission denied` when running a shell script | Add execute permissions, for example with `chmod +x attacks/code_injection/*.sh` |
| `Connection refused` or no response from the Modbus target | Check the target address and port, the network topology, and the firewall and routing rules between the host and the PLC |
| Loading the Docker images fails | Build the images locally with the scripts in `plc_simulations/scripts/mixing/batt_tds/`, since the image archives are not distributed |

## Known limitations

- The `network_simulations/` folder provides configuration files and notes, not a turnkey deployment script.
- The attack tools are research code and use hard-coded PLC addresses, which must be updated for other environments.
- The wrapper scripts in `attacks/ddos/` call a compiled `./dos` binary, which must first be built from `dos.cpp`.

## Responsible use

This repository is a laboratory ICS research artifact. Use it only on systems you own or are explicitly authorized to test.

The attack scripts target OpenPLC instances running inside a GNS3-emulated network. They use the standard, unauthenticated Modbus/TCP interface, a well-documented limitation of the protocol, to write coil and register values, and they exploit no undisclosed vulnerability. They are provided to make the experimental conditions described in the paper reproducible and do not offer attack capabilities beyond those already documented in the Modbus security literature, for example Chang et al., *The Modbus protocol vulnerability test in industrial control systems* (CyberC 2020), and Rahman et al., *Launch of denial of service attacks on the Modbus/TCP protocol and development of its protection mechanisms* (International Journal of Critical Infrastructure Protection, 2022).
