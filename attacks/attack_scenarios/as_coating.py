import random
import time
import subprocess
from typing import Callable


# =========================================================
# Attack Functions
# =========================================================

random_time = random.randint(15, 60)
def bias_injector():

    subprocess.run([
        "python3",
        "-m",
        "bias_injector.bi_coating",
        "--duration", str(random_time),
        "--white",
        "--wn_stddev", str(random.uniform(3, 15)),
        "--brown",
        "--bn_alpha", str(random.uniform(0.5, 5)),
        "--bn_stddev", str(random.uniform(3, 15)),
        "--scan_interval", "2.0",
        "--write_interval", "0.001"
    ])


def coil_manipulation():

    subprocess.run([
        "python3",
        "-m",
        "coil_manipulation.cm_coating",
        "--duration", str(random_time)
    ])


def holding_register_manipulation():

    subprocess.run([
        "python3",
        "-m",
        "holding_register_manipulation.hrm_coating",
        "--duration", str(random_time)
    ])


def desynchronizer():

    subprocess.run([
        "python3",
        "-m",
        "desynchronizer.desync_coating",
        "--duration", str(random_time)
    ])


def plc_reset():

    subprocess.run([
        "python3",
        "-m",
        "plc_reset.reset_plc_coating",
        "--duration", str(random_time)
    ])


def fuzzing():

    subprocess.run([
        "python3",
        "-m",
        "MF.modFuzzer",
        "--grammar",
        "192.168.60.3",
        "--duration",
        str(random_time),
        "-o",
        "MF/fuzzing_sessions",
        "--log_file",
        "log_fuzzing_coating.csv"
    ])

    # Optional PLC reset after fuzzing
    subprocess.run([
        "python3",
        "-m",
        "plc_reset.reset_plc_coating",
        "--duration",
        "1"
    ])


# =========================================================
# Main Scheduler
# =========================================================

def execute_attacks(
    attacks: dict[Callable, float],
    total_execution_time: int,
    starting_attack_time: int,
    end_attack_time: int,
    cooldown_min: int = 240,
    cooldown_max: int = 420
):

    # -----------------------------
    # Validation
    # -----------------------------

    if starting_attack_time >= end_attack_time:
        raise ValueError(
            "starting_attack_time must be < end_attack_time"
        )

    if end_attack_time > total_execution_time:
        raise ValueError(
            "end_attack_time cannot exceed total_execution_time"
        )

    if len(attacks) == 0:
        raise ValueError("attacks dictionary cannot be empty")

    probabilities = list(attacks.values())

    if any(p < 0 for p in probabilities):
        raise ValueError(
            "Probabilities must be non-negative"
        )

    total_probability = sum(probabilities)

    if total_probability <= 0:
        raise ValueError(
            "Sum of probabilities must be > 0"
        )

    # Normalize probabilities
    normalized_probabilities = [
        p / total_probability
        for p in probabilities
    ]

    attack_functions = list(attacks.keys())

    # -----------------------------
    # Start Experiment
    # -----------------------------

    experiment_start = time.time()

    print(f"[INFO] Total execution time: {total_execution_time}s")
    print(f"[INFO] Attacks start at: {starting_attack_time}s")
    print(f"[INFO] Attacks end at: {end_attack_time}s")

    # -----------------------------
    # Initial normal traffic phase
    # -----------------------------

    while True:

        elapsed = time.time() - experiment_start

        if elapsed >= starting_attack_time:
            break

        remaining = int(starting_attack_time - elapsed)

        print(
            f"[NORMAL TRAFFIC] "
            f"Attacks start in {remaining}s",
            end="\r"
        )

        time.sleep(1)

    print("\n[INFO] Attack phase started\n")

    # -----------------------------
    # Attack phase
    # -----------------------------

    while True:

        elapsed = time.time() - experiment_start

        # End experiment
        if elapsed >= total_execution_time:
            print("\n[INFO] Total execution time reached")
            break

        # Stop attacks after attack window
        if elapsed >= end_attack_time:

            remaining = int(
                total_execution_time - elapsed
            )

            print(
                f"[NORMAL TRAFFIC] "
                f"No more attacks "
                f"(remaining: {remaining}s)",
                end="\r"
            )

            time.sleep(1)
            continue

        # -----------------------------
        # Random attack selection
        # -----------------------------

        selected_attack = random.choices(
            population=attack_functions,
            weights=normalized_probabilities,
            k=1
        )[0]

        attack_name = selected_attack.__name__

        current_time = int(elapsed)

        print(
            f"\n[ATTACK STARTED] "
            f"t={current_time}s -> {attack_name}"
        )

        # -----------------------------
        # Execute attack
        # -----------------------------

        try:
            selected_attack()

        except Exception as e:
            print(
                f"[ERROR] Attack {attack_name} failed: {e}"
            )

        print(
            f"[ATTACK FINISHED] {attack_name}"
        )

        # -----------------------------
        # Random cooldown
        # -----------------------------

        cooldown = random.uniform(cooldown_min, cooldown_max)

        print(
            f"[COOLDOWN] Waiting "
            f"{cooldown:.2f}s"
        )

        time.sleep(cooldown)


# =========================================================
# Example Usage
# =========================================================

if __name__ == "__main__":

    attacks = {
        bias_injector: 1/6,
        coil_manipulation: 1/6,
        holding_register_manipulation: 1/6,
        desynchronizer: 1/6,
        plc_reset: 1/6,
        fuzzing: 1/6
    }

    factor = 3600
    cooldown_min = 4*60
    cooldown_max = 7*60
    execute_attacks(
        attacks=attacks,
        total_execution_time=10 * factor,
        starting_attack_time=2 * factor,
        end_attack_time=9 * factor,
        cooldown_min=cooldown_min,
        cooldown_max=cooldown_max
    )