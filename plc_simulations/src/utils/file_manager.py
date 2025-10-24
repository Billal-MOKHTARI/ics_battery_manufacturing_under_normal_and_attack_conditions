
from colorama import Fore, Style, init
import os

def get_experiment_key(filename):
    if os.path.isfile(filename):  # Ensure it's a file
        try:
            if os.path.getsize(filename) > 0:  # Check if file is non-empty
                with open(filename, "r") as f:
                    return f.read().strip()
            else:
                print(Fore.RED + "Error: Experiment key file is empty." + Style.RESET_ALL)
        except Exception as e:
            print(Fore.RED + f"Error reading experiment key: {str(e)}" + Style.RESET_ALL)
    else:
        print(Fore.RED + "Error: Experiment key file does not exist." + Style.RESET_ALL)
    return None

def save_experiment_key(key, filename):
    try:
        with open(filename, "w") as f:
            f.write(key)
    except Exception as e:
        print(Fore.RED + f"Error saving experiment key: {str(e)}" + Style.RESET_ALL)