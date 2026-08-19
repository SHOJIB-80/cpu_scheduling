# cpu_scheduling

Interactive command-line CPU scheduling simulator implemented in C. It runs multiple classic process-scheduling algorithms, prints a Gantt chart and per-process metrics (completion, waiting, turnaround), and can compare algorithms by average waiting time.

## Overview

cpu_scheduling is a small educational simulator for CPU scheduling algorithms. It is intended for students and developers who want to experiment with and compare scheduling strategies (FCFS, SJF, SRTF, Round Robin, Priority, and Round Robin with Priority) using custom process arrival times, burst times, and priorities.

## Features

- Interactive CLI program (single executable).
- Implements these scheduling algorithms:
  - FCFS (First-Come, First-Served)
  - SJF (Shortest Job First) — non-preemptive
  - SRTF (Shortest Remaining Time First) — preemptive
  - Round Robin (time quantum)
  - Priority Scheduling — non-preemptive (lower number = higher priority)
  - Round Robin with Priority
- Prints Gantt chart (text) showing execution segments including idle times.
- Shows per-process metrics: PID, arrival, burst, (priority), completion time, waiting time, turnaround time.
- Reports average waiting time and average turnaround time.
- Option to compare any two algorithms by average waiting time.

## Tech stack

- Language: C
- Build tools: gcc or clang (any standard C compiler)
- No external libraries or runtime dependencies

## Repository structure

```text
/
├── cpu_scheduling_simulator.c   # Main program (all logic in one C file)
└── README.md                    # This file
```

How it fits together:
- cpu_scheduling_simulator.c contains:
  - data structure `Process` for process state,
  - implementations for each algorithm (functions: `fcfs`, `sjf`, `srtf`, `roundRobin`, `priorityScheduling`, `priorityRoundRobin`),
  - helpers for building and printing a Gantt chart and results,
  - an interactive `main()` loop which prompts the user for input and runs algorithms.

## Requirements / Prerequisites

- A C compiler (gcc or clang). Example: gcc version or clang supporting C99.
- POSIX-like terminal (Linux, macOS, WSL). The program is standard console I/O and should also work on Windows if compiled with an appropriate toolchain.

No external libraries, build systems, or runtime services are required.

## Installation (build)

Clone the repository and compile the single C source file:

```bash
git clone https://github.com/SHOJIB-80/cpu_scheduling.git
cd cpu_scheduling

# Compile with GCC (recommended flags)
gcc -std=c99 -O2 -Wall cpu_scheduling_simulator.c -o cpu_scheduling_simulator
```

You can also use clang:

```bash
clang -std=c99 -O2 -Wall cpu_scheduling_simulator.c -o cpu_scheduling_simulator
```

## Configuration

No configuration files or environment variables are required. All inputs are provided interactively at runtime.

## Running the program

Start the simulator:

```bash
./cpu_scheduling_simulator
```

The program shows a menu:

1. FCFS
2. SJF (Non-Preemptive)
3. SRTF (Preemptive)
4. Round Robin
5. Priority Scheduling
6. Round Robin with Priority
7. Compare Two Algorithms
8. Exit

Follow prompts to enter:
- Number of processes (integer, 1–50)
- For each process: burst time (positive integer), arrival time (non-negative integer), priority (integer where smaller means higher priority)
- For Round Robin variants: time quantum (positive integer) when requested

The program will print a textual Gantt chart, then a table with per-process metrics and the average waiting/turnaround times.

## Input details / Usage

When prompted by input routines, the program expects integers:

- Number of processes: must be 1..50
- For each process i:
  - Process i burst time: positive integer (> 0)
  - Process i arrival time: non-negative integer (>= 0)
  - Process i priority: integer (lower number = higher priority)
- Time Quantum (for Round Robin / comparisons): positive integer (> 0)

Notes:
- The program enforces simple checks for burst (>0) and arrival (>=0). It does not validate non-integer input (entering non-integers may cause undefined behavior).
- Maximum number of processes is 50 (constant MAXP in source).
- Maximum number of Gantt segments is 5000 (constant MAXG in source).

Example (interactive prompts shown; user input in bold):

```
Enter your choice: 1
Enter number of processes: 3

Process 1 burst time: 5
Process 1 arrival time: 0
Process 1 priority (lower number = higher priority): 2

Process 2 burst time: 3
Process 2 arrival time: 2
Process 2 priority (lower number = higher priority): 1

Process 3 burst time: 2
Process 3 arrival time: 4
Process 3 priority (lower number = higher priority): 3
```

After entering the data, the program prints a Gantt chart and a table similar to:

- Gantt chart (example format):

  | P1 | P2 | P3 |
  0     5     8     10

- Table header (when priority shown):
  PID  Arr  Burst  Pri  Comp  Wait  Turn

- Average Waiting Time and Average Turnaround Time printed below the table.

## Algorithms / Implementation notes

- FCFS: sorts processes by arrival time and schedules them in arrival order.
- SJF (non-preemptive): selects the available process with the smallest burst time.
- SRTF: preemptive shortest-remaining-time-first — scheduling decisions occur each time unit.
- Round Robin: uses a queue; processes arriving while the CPU is running are enqueued; the time quantum is applied.
- Priority Scheduling: non-preemptive; lower priority value has higher precedence.
- Round Robin with Priority: selects among arrived processes with the best (lowest) priority and cycles among equal-priority processes.

Core functions in the source:
- runAlgorithm(choice, original[], n, tq) — dispatches to chosen algorithm.
- showResults(...) — prints Gantt chart and metrics for a finished run.
- inputProcesses(...) — interactive input and basic validation.

## Limitations / Known issues

- Single-file, synchronous CLI tool intended for learning/demonstration — not designed as a library or service.
- Input must be integers — non-integer input is not guarded against and may produce undefined behavior.
- No automated tests are included.
- No persistence/export: results are printed to standard output only.
- No formal command-line flags; program is purely interactive.
- Max processes: 50. Max Gantt segments: 5000.

## Testing

There are no unit tests included in this repository. To exercise the program, compile and run it and try several algorithm choices and process sets (including edge cases like idle CPU periods).

## Contributing

If you'd like to contribute:

1. Fork the repository.
2. Create a descriptive branch: git checkout -b fix/feature-name
3. Make changes and test locally.
4. Open a pull request describing the change and motivation.

Please include reproducible steps and test cases when proposing behavioral changes (e.g., changes to scheduling logic).

## License

No license file is present in this repository. If you intend to publish or share this code, add a LICENSE file (for example, MIT, Apache-2.0, etc.). TODO: add a license.

## Author / Maintainer

The repository does not include an explicit author or maintainer note in the source. TODO: add author/maintainer and contact information to README or source headers if desired.

## Acknowledgements

This project implements classic scheduling algorithms commonly studied in operating systems courses. No external code or third-party libraries are required.

---

If you want, I can:
- Add example input files and a non-interactive mode (read processes from a file),
- Add a build script (Makefile) and CI checks,
- Create unit tests for the scheduling functions,
- Or add a LICENSE and Author section — tell me which you'd prefer.
