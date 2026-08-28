# CPU Scheduling Simulator

An interactive command-line CPU scheduling simulator implemented in C that demonstrates and compares classic process-scheduling algorithms commonly studied in operating systems courses.

## Overview

This project is an educational tool that allows users to simulate and analyze various CPU scheduling algorithms. The application provides an interactive command-line interface where users can input process details and run multiple scheduling strategies to visualize execution order, calculate performance metrics, and compare algorithm efficiency.

## Features

- **Interactive CLI interface** with menu-driven algorithm selection
- **Six scheduling algorithms**:
  - FCFS (First-Come, First-Served)
  - SJF (Shortest Job First) — non-preemptive
  - SRTF (Shortest Remaining Time First) — preemptive
  - Round Robin with configurable time quantum
  - Priority Scheduling — non-preemptive
  - Round Robin with Priority
- **Gantt chart visualization** displaying process execution segments and idle times
- **Per-process metrics** including arrival time, burst time, completion time, waiting time, and turnaround time
- **Summary statistics** showing average waiting time and average turnaround time
- **Algorithm comparison** functionality to directly compare average waiting times between any two algorithms

## Technology Stack

| Component | Technology  |
| --------- | ----------- |
| Language  | C (C99)     |
| Compiler  | GCC/Clang   |
| Build     | Direct compilation |
| Platform  | POSIX (Linux, macOS, Windows with WSL) |

## Project Structure

```
cpu_scheduling/
├── cpu_scheduling_simulator.c    # Main program with all scheduling logic
└── README.md                     # Documentation
```

The single C source file contains:
- `Process` data structure for managing process state
- Implementations of all six scheduling algorithms
- Gantt chart generation and display functions
- Results display with per-process metrics
- Interactive `main()` loop for user input and algorithm execution

## Installation and Build

Clone the repository and compile:

```bash
git clone https://github.com/SHOJIB-80/cpu_scheduling.git
cd cpu_scheduling
gcc -std=c99 -O2 -Wall cpu_scheduling_simulator.c -o cpu_scheduling_simulator
```

Alternatively, using Clang:

```bash
clang -std=c99 -O2 -Wall cpu_scheduling_simulator.c -o cpu_scheduling_simulator
```

## Running the Program

Start the simulator:

```bash
./cpu_scheduling_simulator
```

The application displays a main menu with eight options:

```
1. FCFS
2. SJF (Non-Preemptive)
3. SRTF (Preemptive)
4. Round Robin
5. Priority Scheduling
6. Round Robin with Priority
7. Compare Two Algorithms
8. Exit
```

## Usage

### Running a Single Algorithm

1. Select an algorithm (1–6) from the main menu
2. Enter the number of processes (1–50)
3. For each process, provide:
   - Burst time (positive integer > 0)
   - Arrival time (non-negative integer ≥ 0)
   - Priority value (integer; lower values indicate higher priority)
4. For Round Robin variants, enter the time quantum (positive integer > 0)
5. The program displays the Gantt chart and per-process metrics

### Comparing Algorithms

1. Select option 7 from the main menu
2. Input process details as above
3. Select the first algorithm to run
4. Select the second algorithm to run
5. If either algorithm requires a time quantum, enter it
6. The program runs both algorithms and displays a side-by-side comparison of average waiting times, indicating which algorithm performed better

### Example Interaction

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

The output includes a Gantt chart and results table:

```
Gantt Chart:
| P1    | P2    | P3    |
0       5       8       10

PID  Arr  Burst  Comp  Wait  Turn
1    0    5      5     0     5
2    2    3      8     3     6
3    4    2      10    4     6

Average Waiting Time: 2.33
Average Turnaround Time: 5.67
```

## Input Specifications

- **Number of processes**: Integer from 1 to 50
- **Burst time**: Positive integer (> 0)
- **Arrival time**: Non-negative integer (≥ 0)
- **Priority**: Integer value (lower number = higher priority)
- **Time Quantum**: Positive integer (> 0), required for Round Robin and Round Robin with Priority

**Note:** The program performs basic validation for positive burst times and non-negative arrival times. Non-integer input is not validated and may cause undefined behavior.

## Algorithm Details

- **FCFS**: Processes are executed in the order of their arrival times
- **SJF (Non-Preemptive)**: Among available processes, the one with the smallest burst time is selected
- **SRTF (Preemptive)**: Scheduling decisions occur each time unit, selecting the process with the smallest remaining time
- **Round Robin**: Processes are executed in cycles; each process runs for at most the time quantum before being re-queued
- **Priority Scheduling (Non-Preemptive)**: Among available processes, the one with the lowest priority value is selected
- **Round Robin with Priority**: Processes with the best (lowest) priority value are selected and cycled through with the time quantum constraint

## Constraints and Limitations

- Maximum of 50 processes per simulation
- Maximum of 5000 Gantt chart segments
- Non-integer input is not validated and may result in undefined behavior
- No automated test suite included
- Results are output to the console only; no export or persistence functionality
- No command-line arguments; program is purely interactive
- Single-threaded, synchronous execution

## Requirements

- C compiler supporting C99 standard (GCC or Clang)
- POSIX-compatible terminal environment (Linux, macOS, WSL on Windows)
- Standard C library (no external dependencies)

## License

No license file is currently included in this repository.

## Author

SHOJIB-80

---

This project is designed as an educational tool for students and developers studying CPU scheduling algorithms in operating systems courses.
