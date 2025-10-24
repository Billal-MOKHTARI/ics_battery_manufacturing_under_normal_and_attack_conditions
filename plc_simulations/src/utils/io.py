import json
from pathlib import Path
import subprocess
import os

def read_json(file_path: str):
    path = Path(file_path)
    if not path.exists():
        raise FileNotFoundError(f"File not found: {file_path}")
    
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except json.JSONDecodeError as e:
        raise ValueError(f"Invalid JSON format in {file_path}: {e}")
    


def clear_terminal():
    subprocess.run("cls" if os.name == "nt" else "clear", shell=True)
