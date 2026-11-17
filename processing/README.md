# Processing

`process.py` builds `historical_data_processed.csv` from the raw operational
records (`historical_data.csv`) and the attack logs of one process of the
CyBattICS dataset (coating, or slitting and notching).

## Files

| File | Content |
| --- | --- |
| `process.py` | Column selection, expected signal values, attack labels |
| `reference_signals.py` | Reference trajectories s_i(t) of every SFC step (Table 3 of the paper) |
| `requirements.txt` | Python dependencies |

## Usage

Download the dataset from Zenodo (DOI 10.5281/zenodo.22086671), then run:

```bash
cd processing
pip install -r requirements.txt

python process.py --process coating           --data-root <dataset folder> --out coating_processed.csv
python process.py --process slitting_notching --data-root <dataset folder> --out slitting_notching_processed.csv
```

`<dataset folder>` is the folder that contains `coating_data/` and
`slitting_notching_data/`. The script does not overwrite an existing output
file unless `--overwrite` is given.

## What the script does

1. **Columns.** Keeps `id`, `signalX*`, `outX*` and `created_at`, renamed
   `obs_signal_value(X*)`, `status(X*)` and `timestamp`. `id` is renumbered
   from 0.
2. **Expected values.** For each step X_i,
   `exp_signal_value(X_i) = s_i(t - t_on)` while `status(X_i) = 1`, and 0
   otherwise. t_on is the timestamp of the last 0 to 1 transition of
   `status(X_i)`, and s_i is defined in `reference_signals.py`.
3. **Labels.** `attack_type` is the attack type of the window that contains
   the record timestamp (bounds included), and `normal` otherwise.
   * Attack logs give `start_time` and `end_time`.
   * The grammar-fuzzing log gives the end of each session (`timestamp`) and
     its duration, so the window is `[timestamp - duration_sec, timestamp]`.
   * The 1-s PLC reset run after each fuzzing session (logged in
     `log_deadlock.csv`) is labelled as part of that fuzzing session.

## Notes on the expected values

* Timestamps have a 1-s resolution, so elapsed times are whole seconds.
* The PLC evaluates s_i on an internal clock that advances by 0.1 every 20-ms
  cycle, while this script uses wall-clock seconds. Transients therefore do
  not match exactly. Constant segments are not affected.
* When the signal generator of an active step has terminated (`termX_i = 1`
  in the raw file), the PLC signal drops to its noise terms while the expected
  value keeps following s_i.
* For X9 of the slitting-and-notching PLC, the PLC program uses 3.0 instead of
  pi. `reference_signals.py` uses pi.

## Reproducibility

Tested with Python 3.13, pandas 3.0.5 and numpy 2.5.3. The output is
identical byte for byte to the processed files of the revised dataset version.
