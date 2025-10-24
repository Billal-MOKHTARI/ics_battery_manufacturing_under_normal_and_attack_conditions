import time
from plc_simulations.src.services.system.mixing.batt_tds.connection import client, physical_addr, sensors

while True:
    for key in physical_addr.keys():

        f_value = client.read_coils(address=physical_addr["f"], count=1).bits[0]
        l_value = client.read_coils(address=physical_addr["l"], count=1).bits[0]
        e2_value = client.read_coils(address=physical_addr["e2"], count=1).bits[0]
        d2_value = client.read_coils(address=physical_addr["d2"], count=1).bits[0]
        if f_value and l_value:
            time.sleep(1)
            client.write_coil(physical_addr["f"], False)
            client.write_coil(physical_addr["l"], False)

        if d2_value and e2_value:
            time.sleep(1)
            client.write_coil(physical_addr["d2"], False)
            client.write_coil(physical_addr["e2"], False)

        if key in sensors and key not in ["f", "l", "e2", "d2"]:
            value = client.read_coils(address=physical_addr[key], count=1).bits[0]
            if value:
                time.sleep(1)  # Wait 1 second before turning it off
                client.write_coil(physical_addr[key], False)