import time
from plc_simulations.src.services.system.mixing.batt_tds.connection import client, physical_addr

sequence = [["m"], ["d1", "e1"], ["a", "b"], ["d2", "e2"]]
t = 1

while True:
    PV = client.read_holding_registers(address=physical_addr["PV"], count=1).registers[0]
    for sensor in sequence:
        for e in sensor:
            client.write_coil(physical_addr[e], True)
        time.sleep(t)

    for i in range(PV):
        time.sleep(client.read_holding_registers(address=physical_addr["timer_gauge_1"], count=1).registers[0])
        client.write_coil(physical_addr["f"], True)
        client.write_coil(physical_addr["l"], True)
        time.sleep(client.read_holding_registers(address=physical_addr["timer_gauge_2"], count=1).registers[0])
        client.write_coil(physical_addr["g"], True)