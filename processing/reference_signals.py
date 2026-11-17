"""Deterministic reference trajectories s_i(t) of the SFC steps (Table 3 of the paper).

Each function returns the nominal value of the process signal of step X_i, t seconds
after the step was activated. They are the same functions as the ``functionX*``
blocks of the PLC programs in ``plc_simulations/plc_programs/``.

Note: in the slitting-and-notching PLC program, ``functionX9`` uses PI := 3.0.
The function below uses math.pi, as in the released processed data.
"""

import math

COATING = {
    "X0": lambda t: 1.0,
    "X1": lambda t: 100.0 * (1 - math.exp(-t)),
    "X2": lambda t: 150.0 * (t / 2.0) if t < 2.0 else 150.0,
    "X3": lambda t: 8.0,
    "X4": lambda t: 150.0 * (1 + 0.05 * math.sin(2 * math.pi * 0.2 * t)),
    "X5": lambda t: 2.5 * (1 - math.exp(-t / 0.5)),
    "X6": lambda t: 100.0 * (t / 0.8) if t < 0.8 else 100.0,
    "X7": lambda t: 8.0 * (1 - math.exp(-t / 1.5)),
    "X8": lambda t: 140.0,
    "X9": lambda t: 120.0 * (1 - math.exp(-t / 3.0)),
    "X10": lambda t: 5.0 * (t / 2.0) if t < 2.0 else 5.0,
    "X11": lambda t: 160.0 * math.exp(-t / 4.0) + 140.0 * (1 - math.exp(-t / 4.0)),
    "X12": lambda t: 4.0 * (1 - math.cos(math.pi * t / 10.0)) / 2.0,
    "X13": lambda t: 5.0,
    "X14": lambda t: 130.0 + 5.0 * math.sin(2 * math.pi * 0.5 * t),
    "X15": lambda t: 1.0,
}

SLITTING_NOTCHING = {
    "X0": lambda t: 1.0,
    "X1": lambda t: 150.0 * (1 - math.exp(-t / 2.0)),
    "X2": lambda t: 5.0 * (t / 1.5) if t < 1.5 else 5.0,
    "X3": lambda t: 10.0 * (1 - math.cos(math.pi * t / 2.0)) / 2.0,
    "X4": lambda t: 180.0 * (1 - math.exp(-t / 1.2)) + 5.0 * math.sin(2 * math.pi * 0.3 * t),
    "X5": lambda t: 100.0 * (t / 1.0) if t < 1.0 else 100.0,
    "X6": lambda t: 100.0 * (1 - math.exp(-t / 0.8)),
    "X7": lambda t: 30.0 * (t / 2.5) ** 2 if t < 2.5 else 30.0,
    "X8": lambda t: 500.0 * (1 - math.exp(-t / 1.0)),
    "X9": lambda t: 120.0 * (1 + 0.1 * math.sin(2 * math.pi * 0.2 * t)),
    "X10": lambda t: 1.0,
    "X11": lambda t: 100.0 * math.exp(-t / 3.0) + 80.0 * (1 - math.exp(-t / 3.0)),
    "X12": lambda t: 5.0 * math.sin(math.pi * t / 2.5) / 2.0,
    "X13": lambda t: 10.0 * math.sin(2 * math.pi * 1.0 * t),
    "X14": lambda t: 2.0 * (t / 2.0) if t < 2.0 else 2.0,
    "X15": lambda t: 1.0 * (1 - math.exp(-t / 0.5)),
    "X16": lambda t: 50.0 * (t / 0.8) if t < 0.8 else 50.0,
    "X17": lambda t: 10.0,
    "X18": lambda t: 30.0 * (1 - math.exp(-t / 0.2)),
    "X19": lambda t: 8.0,
    "X20": lambda t: 60.0 * (t / 1.0) if t < 1.0 else 60.0,
    "X21": lambda t: 5.0,
    "X22": lambda t: 1.0 if t < 0.5 else 0.0,
}

REFERENCE_SIGNALS = {
    "coating": COATING,
    "slitting_notching": SLITTING_NOTCHING,
}
