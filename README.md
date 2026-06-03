# ICS Battery Manufacturing Simulation and Attack Framework

This repository is a research-grade framework for modeling an industrial battery manufacturing environment under both normal and attack conditions.
It combines static network configuration material, PLC Modbus simulation services, and a suite of attack tools for ICS security experiments.

## What is included

- `network_simulations/`
  - Device and network topology configuration examples
  - Cisco router/switch configuration snippets
  - ELK stack, MySQL server, OpenPLC, and SCADA notes
  - Sample credentials and container metadata in JSON

- `plc_simulations/`
  - PLC Modbus service code and a small GUI simulator
  - Docker image build helpers for mixing PLC services
  - PLC data logger and monitor programs
  - Static PLC address mapping in `configs/vars.json`

- `attacks/`
  - Modbus/PLC attack scripts for code injection, bias injection, coil manipulation, holding register attacks, desynchronization, and PLC reset
  - Fuzzing tool in `attacks/MF/` for grammar-based Modbus fuzzing
  - Experiment runners in `attacks/attack_scenarios/`
  - Cleanup helper `attacks/cleanup.sh`

## Architecture overview

This repository is organized into three main domains:

1. **Network simulation artifacts** (`network_simulations/`)
   - Contains network design, firewall/router credentials, and ELK deployment notes.
   - This folder is primarily configuration documentation and is not a single deployable service.

2. **PLC simulation and service code** (`plc_simulations/`)
   - `src/services/system/mixing/batt_tds/`: Modbus client services built around `configs/vars.json`
   - `src/services/docker/mixing/batt_tds/`: container-ready scripts for runtime, auto-start, logger, monitor, and parameter configuration
   - `src/utils/io.py`: utility code for JSON loading and terminal clearing
   - `scripts/mixing/batt_tds/`: Docker build helper scripts
   - `jobs/load_docker_images.sh`: loads prebuilt image archives (note: image tarballs are not included in this repository)

3. **Attack campaigns** (`attacks/`)
   - `code_injection/`: a Modbus code injection attack tool with CLI and interactive wrappers
   - `bias_injector/`, `coil_manipulation/`, `holding_register_manipulation/`, `desynchronizer/`, `plc_reset/`: targeted attack modules for PLC variables
   - `attack_scenarios/`: orchestrated experiment runners that execute a randomized attack sequence
   - `MF/`: grammar-based Modbus fuzzing engine with its own requirements and CLI

## Quick start

### Prerequisites

- Linux or macOS with `python3` installed
- `pip3` for Python package installation
- `docker` and `docker buildx` if you want to build container images
- `nmap` for the attack wrapper scripts that scan a subnet

### Install Python dependencies

From the repository root:

```bash
python3 -m pip install --upgrade pip
python3 -m pip install -r plc_simulations/requirements.txt
python3 -m pip install pymodbus colorama
```

If you want to run the fuzzing tool in `attacks/MF/`:

```bash
cd attacks/MF
python3 -m pip install -r requirements.txt
cd -
```

Optional packages for specific services:

```bash
python3 -m pip install mysql-connector-python netifaces
python3 -m pip install comet_ml
sudo apt install nmap
```

### Build PLC Docker images (optional)

```bash
cd plc_simulations
./scripts/mixing/batt_tds/build_simulator.sh
./scripts/mixing/batt_tds/build_runtime.sh
./scripts/mixing/batt_tds/build_auto_start.sh
./scripts/mixing/batt_tds/build_data_logger.sh
./scripts/mixing/batt_tds/build_data_monitor.sh
./scripts/mixing/batt_tds/configure_parameters.sh
cd -
```

### Load image archives (optional)

The repository includes `plc_simulations/jobs/load_docker_images.sh` to load saved images from `plc_simulations/docker/images/`.

```bash
cd plc_simulations
bash jobs/load_docker_images.sh
```

> Note: The referenced image tarballs are not present in this repository. This step may fail unless the images are supplied separately. Needs Verification.

## Common entry points

### PLC runtime and service examples

- `plc_simulations/main.py`
  - A simple Modbus client example that connects to `192.168.122.27:502` and writes coil `0`.
- `plc_simulations/src/services/system/mixing/batt_tds/runtime.py`
  - A steady-state PLC runtime that updates coils based on configured physical addresses.
- `plc_simulations/src/services/docker/mixing/batt_tds/runtime.py`
  - Container entrypoint for an interactive PLC runtime loop.
- `plc_simulations/src/services/docker/mixing/batt_tds/data_logger.py`
  - Logs Modbus values into a MySQL database.
- `plc_simulations/src/services/docker/mixing/batt_tds/data_monitor.py`
  - Streams PLC sensor values to Comet ML.
- `plc_simulations/src/services/docker/mixing/batt_tds/configure_parameters.py`
  - Lets an operator update PLC timers and cycle counts via Modbus.
- `plc_simulations/src/services/docker/mixing/batt_tds/auto_start.py`
  - Simulates auto-start PLC behavior by toggling coils over time.

### Attack modules

- `attacks/code_injection/code_injection.py`
  - Modbus code injection attack script targeting coils and holding registers.
- `attacks/code_injection/code_injection_cli.sh`
  - Network scanning wrapper that discovers Modbus hosts and launches `code_injection.py` against each.
- `attacks/code_injection/code_injection_interactive.sh`
  - Interactive wrapper for the same attack.
- `attacks/attack_scenarios/as_coating.py`
  - Runs a randomized attack schedule for coating-line experiments.
- `attacks/attack_scenarios/as_slitting_notching.py`
  - Runs a randomized attack schedule for slitting/notching experiments.
- `attacks/MF/modFuzzer.py`
  - Grammar-based Modbus fuzzing CLI.

## Example workflows

### Run a code injection attack

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

### Run the attack scenario runner

```bash
python3 attacks/attack_scenarios/as_coating.py
```

### Run grammar-based Modbus fuzzing

```bash
cd attacks/MF
python3 modFuzzer.py --grammar 192.168.60.3 --duration 60 --log_file sessions.csv --output_dir ./reports
```

### Run a PLC data logger directly

```bash
python3 plc_simulations/src/services/docker/coating/data_logger.py
```

## Configuration sources

### Hard-coded values

Most of the runtime configuration is currently embedded in code and JSON files rather than environment variables.

- `plc_simulations/configs/vars.json`
  - `connection.ip_address`
  - `connection.port`
  - `physical_addressing`
  - sensor and actuator address maps
- `plc_simulations/src/services/docker/coating/data_logger.py`
  - `MODBUS_IP = "192.168.60.3"`
  - `MODBUS_PORT = 502`
  - `SQL_IP = "192.168.70.4"`
  - `SQL_PORT = 3306`
  - `SQL_USER = "data_logger"`
  - `SQL_PASSWORD = "*F645AE27B7F3EE07F39C5FC96B0B2410CB1D2BEF"`
  - `DATABASE = "production"`
- `attacks/bias_injector/bi_coating.py`
  - `ModbusTcpClient(host="192.168.60.3", port=502)`
- `attacks/attack_scenarios/as_coating.py`
  - uses `192.168.60.3`
- `attacks/attack_scenarios/as_slitting_notching.py`
  - uses `192.168.60.4`

### Environment variables

- No runtime environment variables are directly consumed by the Python attack or PLC service code.
- One configuration script in `network_simulations/configs/ubuntu_toolbox/config.sh` exports `JAVA_HOME`.

## Required vs optional configuration

### Required

- Python 3.x and the packages listed in `plc_simulations/requirements.txt`
- `pymodbus` and `colorama` for attack scripts
- A reachable Modbus TCP target on the expected network (common target addresses are `192.168.60.3` and `192.168.60.4`)
- `nmap` if using the attack wrapper scripts in `attacks/code_injection`

### Optional

- `mysql-connector-python` and `netifaces` for PLC data logger experiments
- `comet_ml` for the data monitor in `plc_simulations/src/services/docker/mixing/batt_tds/data_monitor.py`
- Docker and Docker Buildx for container image builds
- `attacks/MF` dependencies for advanced fuzzing

## Troubleshooting

### Common problems

- `ModuleNotFoundError: No module named 'pymodbus'`
  - Install `pip3 install pymodbus`

- `python3: command not found`
  - Ensure `python3` is installed and on your `PATH`

- Attack wrappers fail to discover PLCs
  - Install `nmap`
  - Validate the target subnet and port
  - Check that the PLC is reachable from your host

- `Permission denied` when running shell scripts
  - Add execute permissions: `chmod +x attacks/code_injection/*.sh`

- `Connection refused` or no response from Modbus target
  - Verify the target IP/port
  - Confirm the network topology matches the hard-coded addresses
  - Check firewalls and routing between host and PLC

- Docker image loading fails
  - The repository does not include the tar archives referenced by `plc_simulations/jobs/load_docker_images.sh`
  - Build images locally with the `plc_simulations/scripts/mixing/batt_tds/*.sh` scripts instead

### Notes and caveats

- The network simulation folder contains configuration examples and notes but not a single turnkey deployment script.
- The attack code is research-oriented and often uses hard-coded PLC targets. Update IP addresses before using in a different environment.
- The `attacks/ddos/` helper scripts refer to a `./dos` binary and may require manual completion. Needs Verification.

## Next steps

- Start with `attacks/code_injection/code_injection.py` for direct Modbus attack testing.
- Use `plc_simulations/configs/vars.json` as the primary PLC mapping source.
- If you want a safe testbed, mirror the `192.168.60.x` network topology shown in the repository.

---

> This project is a laboratory-grade ICS research artifact. Use only on systems you own or have explicit permission to test.

> The attack scripts in the public repository target an OpenPLC instance running inside a GNS3-emulated network. They use the standard, unauthenticated Modbus/TCP interface, a well-documented and widely understood limitation of the protocol, to write coil and register values. This exploits no undisclosed vulnerability. The scripts are included to allow reproducibility of the experimental conditions described in the paper; they do not provide new attack capabilities beyond what is already publicly documented in the Modbus security literature (e.g., \cite{chang2020modbus,rahman2022launch}).