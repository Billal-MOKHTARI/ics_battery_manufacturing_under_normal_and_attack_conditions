from pymodbus.client import ModbusTcpClient

ip_address = '192.168.122.27'
port = 502

client = ModbusTcpClient(ip_address, port=port)
connection_status = client.connect()

while True:
    client.write_coil(0, 1)
