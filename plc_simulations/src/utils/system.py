import netifaces
from colorama import Fore, Style, init
import mysql.connector
def exec_sql(sql, conn, cur, fetch=True):
    """Execute SQL command, commit changes, and optionally return results."""
    try:
        cur.execute(sql)
        
        # Check if the query should return results (e.g., SELECT statements)
        if fetch and cur.with_rows:
            result = cur.fetchall()  # Fetch all results

        else:
            result = None  # No result for INSERT, UPDATE, DELETE

        conn.commit()  # Commit changes for transactional queries
        return result  # Return result if applicable

    except mysql.connector.Error as err:
        print(Fore.RED + f"SQL Error: {err}" + Style.RESET_ALL)
        conn.rollback()
        return None  # Return None on error


# Get the IP address and subnet mask of the Linux machine
def get_linux_ip_and_mask():
    """Get Linux machine's IPv4 address and subnet mask"""
    for interface in netifaces.interfaces():
        addrs = netifaces.ifaddresses(interface)
        if netifaces.AF_INET in addrs:
            for addr in addrs[netifaces.AF_INET]:
                if not addr['addr'].startswith('127.'):  # Skip loopback
                    return addr['addr'], addr['netmask']
    return None, None

def mask_to_cidr(mask_str):
    """Convert subnet mask (e.g., 255.255.255.0) to CIDR (e.g., 24)"""
    cidr = 0
    for octet in mask_str.split('.'):
        binary = bin(int(octet))[2:].zfill(8)
        cidr += binary.count('1')
    return cidr

