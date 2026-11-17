"""Generate historical_data_processed.csv from historical_data.csv and the attack logs.

Usage
-----
    python process.py --process coating --data-root ../../data --out coating_processed.csv
    python process.py --process slitting_notching --data-root ../../data --out slitting_notching_processed.csv

Steps
-----
1. Select and rename columns
       signalX*   -> obs_signal_value(X*)
       outX*      -> status(X*)
       created_at -> timestamp
   and renumber ``id`` from 0.
2. Expected values. For each step X_i,
       exp_signal_value(X_i) = s_i(t - t_on)   while status(X_i) == 1
                             = 0               otherwise
   where t_on is the timestamp of the last 0 -> 1 transition of status(X_i)
   and s_i is the reference trajectory in ``reference_signals.py``.
3. Attack labels (``attack_type``). A record takes the type of every attack
   window that contains its timestamp (bounds included), otherwise ``normal``.
     * Standard logs give ``start_time``, ``end_time`` and ``attack_type``.
     * The grammar-fuzzing log gives the session end (``timestamp``) and its
       ``duration_sec``, so the window is [timestamp - duration_sec, timestamp].
     * The 1-s PLC reset that the orchestrator runs after each fuzzing session
       is logged in log_deadlock.csv. It is merged into its fuzzing session.
     * Standard windows are applied first (files in sorted order), fuzzing
       windows last, so the result does not depend on the file system.
"""

import argparse
import glob
import io
import os
import re

import pandas as pd

from reference_signals import REFERENCE_SIGNALS

PROCESS_FOLDERS = {
    "coating": "coating_data",
    "slitting_notching": "slitting_notching_data",
}

# A post-fuzzing reset lasts about 1.1 to 1.3 s and starts within a few
# seconds after the end of the fuzzing session it belongs to.
RESET_MAX_DURATION_S = 5.0
RESET_MATCH_BEFORE_S = 1.0
RESET_MATCH_AFTER_S = 5.0


# ----------------------------------------------------------------------------
# Step 1 and 2: columns and expected values
# ----------------------------------------------------------------------------

def select_columns(raw: pd.DataFrame) -> pd.DataFrame:
    """Keep id, signalX*, outX* and created_at, with their processed names."""
    signal_cols = [c for c in raw.columns if re.match(r"^signalX\d+$", c)]
    out_cols = [c for c in raw.columns if re.match(r"^outX\d+$", c)]

    df = raw[["id"] + signal_cols + out_cols + ["created_at"]].copy()
    renames = {c: f"obs_signal_value({c[len('signal'):]})" for c in signal_cols}
    renames.update({c: f"status({c[len('out'):]})" for c in out_cols})
    renames["created_at"] = "timestamp"
    df = df.rename(columns=renames)

    df["id"] = range(len(df))
    df["timestamp"] = pd.to_datetime(df["timestamp"])
    return df


def expected_signal(status: pd.Series, timestamps: pd.Series, reference) -> list:
    """Evaluate the reference trajectory at the time elapsed since activation.

    A rising edge is a record with status 1 whose previous status is 0 (or the
    first record). A missing previous status does not count as 0.
    """
    values = []
    activation_time = None
    previous = None
    for current, time in zip(status.tolist(), timestamps.tolist()):
        if current == 1 and (previous is None or previous == 0):
            activation_time = time
        if current == 1 and activation_time is not None:
            values.append(reference((time - activation_time).total_seconds()))
        else:
            values.append(0.0)
        previous = current
    return values


def add_expected_values(df: pd.DataFrame, references: dict) -> pd.DataFrame:
    for step, reference in references.items():
        status_col = f"status({step})"
        if status_col not in df.columns:
            print(f"[WARNING] {status_col} not found, {step} skipped")
            continue
        df[f"exp_signal_value({step})"] = expected_signal(df[status_col], df["timestamp"], reference)

    other = [c for c in df.columns if c not in ("id", "timestamp")]
    return df[["id", "timestamp"] + other]


# ----------------------------------------------------------------------------
# Step 3: attack labels
# ----------------------------------------------------------------------------

def is_fuzzing_log(log: pd.DataFrame) -> bool:
    columns = set(log.columns)
    return {"timestamp", "duration_sec"} <= columns and "start_time" not in columns


def read_attack_windows(log_folder: str):
    """Return (standard windows, fuzzing windows) read from the attack logs."""
    log_files = sorted(glob.glob(os.path.join(log_folder, "*.csv")))
    if not log_files:
        raise FileNotFoundError(f"No attack log found in {log_folder}")

    standard, fuzzing = [], []
    for path in log_files:
        log = pd.read_csv(path)
        name = os.path.basename(path)

        if is_fuzzing_log(log):
            end = pd.to_datetime(log["timestamp"], errors="coerce")
            duration = pd.to_numeric(log["duration_sec"], errors="coerce")
            valid = end.notna() & duration.notna()
            for e, d in zip(end[valid], duration[valid]):
                fuzzing.append([e - pd.to_timedelta(d, unit="s"), e])
            print(f"[INFO] {name}: {int(valid.sum())} fuzzing sessions")
            continue

        required = {"attack_type", "start_time", "end_time"}
        if not required <= set(log.columns):
            print(f"[WARNING] {name}: missing {sorted(required - set(log.columns))}, skipped")
            continue

        start = pd.to_datetime(log["start_time"], errors="coerce")
        end = pd.to_datetime(log["end_time"], errors="coerce")
        if "duration_seconds" in log.columns:
            duration = pd.to_numeric(log["duration_seconds"], errors="coerce")
        else:
            duration = (end - start).dt.total_seconds()
        for s, e, d, kind in zip(start, end, duration, log["attack_type"].astype(str)):
            if pd.notna(s) and pd.notna(e):
                standard.append((s, e, kind, d))
        print(f"[INFO] {name}: {len(log)} attack instances")

    return standard, fuzzing


def merge_post_fuzzing_resets(standard: list, fuzzing: list) -> list:
    """Attach each short post-fuzzing PLC reset to its fuzzing session."""
    kept, merged = [], 0
    for start, end, kind, duration in standard:
        session = None
        if kind == "deadlock" and pd.notna(duration) and duration < RESET_MAX_DURATION_S:
            for window in fuzzing:
                lower = window[1] - pd.Timedelta(seconds=RESET_MATCH_BEFORE_S)
                upper = window[1] + pd.Timedelta(seconds=RESET_MATCH_AFTER_S)
                if lower <= start <= upper:
                    session = window
                    break
        if session is None:
            kept.append((start, end, kind))
        else:
            session[1] = max(session[1], end)
            merged += 1
    print(f"[INFO] {merged} post-fuzzing PLC resets merged into fuzzing sessions")
    return kept


def add_attack_labels(df: pd.DataFrame, log_folder: str) -> pd.DataFrame:
    standard, fuzzing = read_attack_windows(log_folder)
    standard = merge_post_fuzzing_resets(standard, fuzzing)

    df["attack_type"] = "normal"
    windows = standard + [(s, e, "fuzzing") for s, e in fuzzing]
    for start, end, kind in windows:
        mask = (df["timestamp"] >= start) & (df["timestamp"] <= end)
        df.loc[mask, "attack_type"] = kind
    return df


# ----------------------------------------------------------------------------
# Pipeline and command line
# ----------------------------------------------------------------------------

def csv_round_trip(df: pd.DataFrame) -> pd.DataFrame:
    """Write and re-read the table as CSV before labelling.

    The released files were produced in two passes (expected values, then
    labels) through an intermediate CSV file. Re-parsing the floats changes the
    last digit of some expected values (below 1e-15). This step reproduces the
    released files byte for byte.
    """
    buffer = io.StringIO()
    df.to_csv(buffer, index=False)
    buffer.seek(0)
    df = pd.read_csv(buffer)
    df["timestamp"] = pd.to_datetime(df["timestamp"])
    return df


def process(process_name: str, data_root: str) -> pd.DataFrame:
    folder = os.path.join(data_root, PROCESS_FOLDERS[process_name])
    raw_path = os.path.join(folder, "historical_data", "historical_data.csv")
    log_folder = os.path.join(folder, "attack_logs")

    print(f"[INFO] Reading {raw_path}")
    df = select_columns(pd.read_csv(raw_path))
    df = add_expected_values(df, REFERENCE_SIGNALS[process_name])
    df = csv_round_trip(df)
    df = add_attack_labels(df, log_folder)
    print(f"[INFO] Labels: {df['attack_type'].value_counts().to_dict()}")
    return df


def main():
    parser = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    parser.add_argument("--process", required=True, choices=sorted(PROCESS_FOLDERS))
    parser.add_argument("--data-root", default=os.path.join("..", "..", "data"),
                        help="folder that contains coating_data/ and slitting_notching_data/")
    parser.add_argument("--out", required=True, help="output CSV file")
    parser.add_argument("--overwrite", action="store_true",
                        help="replace the output file if it already exists")
    args = parser.parse_args()

    if os.path.exists(args.out) and not args.overwrite:
        raise SystemExit(f"{args.out} already exists, use --overwrite to replace it")

    df = process(args.process, args.data_root)
    os.makedirs(os.path.dirname(os.path.abspath(args.out)), exist_ok=True)
    df.to_csv(args.out, index=False)
    print(f"[INFO] Saved {args.out}")


if __name__ == "__main__":
    main()
