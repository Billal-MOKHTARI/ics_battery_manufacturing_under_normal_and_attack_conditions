import struct
from pymodbus.client import ModbusTcpClient
from colorama import Fore, Style
import random

def read_memory(client, address):
    """Read a 32-bit float from two consecutive holding registers."""
    result = client.read_holding_registers(address=address, count=2)
    if result.isError():
        return None
    packed = struct.pack('!HH', result.registers[0], result.registers[1])
    value = struct.unpack('!f', packed)[0]
    return round(value, 3)

def write_memory(client, address, value):
    """Write a 32-bit float into two consecutive holding registers."""
    packed = struct.pack('!f', value)
    regs = struct.unpack('!HH', packed)
    result = client.write_registers(address=address, values=regs)
    if result.isError():
        print(Fore.RED + f"Error writing to memory at address {address}" + Style.RESET_ALL)

def get_enabled_coils(client: ModbusTcpClient, addresses: list) -> list:
    """Return list of coil addresses that are currently True (enabled)."""
    enabled = []
    for addr in addresses:
        result = client.read_coils(address=addr, count=1)
        if not result.isError() and result.bits[0]:
            enabled.append(addr)
    return enabled

def random_dict_from_list(elements, n):
    """
    Randomly select n elements from a list and assign
    each selected element a random value (0 or 1).

    Parameters:
        elements (list): input list
        n (int): number of elements to select

    Returns:
        dict
    """
    
    if n > len(elements):
        raise ValueError("n cannot be larger than the list size")

    selected = random.sample(elements, n)

    result = {
        elem: random.choice([0, 1])
        for elem in selected
    }

    return result

import random


def random_float_dict(l, n, min_val=0.0, max_val=1.0):
    """
    Randomly select n elements from list l and assign
    each selected element a random float value.

    Parameters:
        l (list): input list
        n (int): number of elements to select
        min_val (float): minimum random float
        max_val (float): maximum random float

    Returns:
        dict: {selected_element: random_float}
    """
    
    if n > len(l):
        raise ValueError("n cannot be greater than the list size")

    selected_elements = random.sample(l, n)

    result = {
        elem: random.uniform(min_val, max_val)
        for elem in selected_elements
    }

    return result

