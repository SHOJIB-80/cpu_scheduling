# Schedulab

[Live Preview](https://shojib-80.github.io/cpu_scheduling/)

This project is a browser-based CPU scheduling simulator designed to visualize and compare classic operating system scheduling algorithms. The application lets users define process workloads, run each algorithm locally in the browser, and inspect timing metrics such as waiting time, turnaround time, response time, and CPU utilization without requiring a backend or database.

## Overview

The system provides an interactive simulation environment for six standard CPU scheduling strategies: FCFS, SJF, SRTF, Round Robin, Priority Scheduling, and Round Robin + Priority. It is built as a static web application that runs entirely on the client side, using JavaScript to perform the scheduling calculations and render the output.

Users can simulate a single algorithm or compare two algorithms against the same dataset to evaluate performance differences. The interface includes editable process inputs, validation checks, a Gantt chart view, and summary statistics for each run.

## Features

- Single-algorithm simulation mode
- Dual-algorithm comparison mode
- Six scheduling algorithms:
  - FCFS
  - SJF
  - SRTF
  - Round Robin
  - Priority Scheduling
  - Round Robin + Priority
- Editable process input table with:
  - Process ID
  - Arrival time
  - Burst time
  - Priority
- Configurable time quantum for Round Robin modes
- Gantt chart visualization of execution order and idle periods
- Per-process metric output:
  - Completion time
  - Turnaround time
  - Waiting time
  - Response time
- Summary metrics:
  - Average waiting time
  - Average turnaround time
  - Average response time
  - CPU utilization
- Local CSV export of results
- Browser-only execution with no server dependency
- Input validation for process count, values, and quantum

## Technology Stack

| Technology | Purpose |
|---|---|
| HTML5 | Application structure and interface layout |
| CSS | Styling and responsive dashboard layout |
| JavaScript (ES modules) | Scheduling logic, UI updates, and result rendering |
| Node.js | Test execution for the repository test suite |
| Original C implementation | Reference/legacy source for the scheduling algorithms |

## Project Structure

```text
cpu_scheduling/
├── css/
│   └── styles.css
├── js/
│   ├── app.mjs
│   └── scheduler.mjs
├── original/
│   └── cpu_scheduler.c
├── tests/
│   └── scheduler.test.mjs
├── index.html
├── README.md
└── ...
```

The main application entry point is `index.html`. The browser logic is split between `js/scheduler.mjs`, which contains the scheduling algorithms and validation rules, and `js/app.mjs`, which renders the interface and handles user interactions. The `original/` directory contains the historical C implementation that the JavaScript version was adapted from. The `tests/` directory contains automated checks for algorithm correctness and input validation.

## System Architecture

```text
User
  ↓
Browser UI
  ↓
JavaScript Scheduler
  ↓
Local Process Simulation
  ↓
Metrics + Gantt Chart Output
```

The application does not use a backend service, database, or external API. The simulation runs entirely in the browser using JavaScript functions that process the process list and generate the output.

## Installation

There is no package installation step for the main application because it is a static front-end project.

1. Clone the repository:
   ```bash
   git clone https://github.com/SHOJIB-80/cpu_scheduling.git
   cd cpu_scheduling
   ```

2. Open `index.html` directly in a browser, or serve the project locally:

   ```bash
   python3 -m http.server 8000
   ```

3. Visit:
   ```text
   http://localhost:8000
   ```

## Configuration

The application does not require environment variables, database configuration, or a `.env` file. The simulator uses the process data entered in the UI and the time quantum value configured for Round Robin-based algorithms.

The scheduling rules and validation logic are defined in `js/scheduler.mjs`, and the interface behavior is handled in `js/app.mjs`.

## Running the Project

1. Start a local web server from the project root, if needed.
2. Open the application in a browser.
3. Select the desired simulation mode.
4. Choose a scheduling algorithm or compare two algorithms.
5. Edit the process set and time quantum.
6. Run the simulation to view the results.

The project is ready to use without any additional setup beyond a browser and, optionally, a simple local static server.

## Usage

### Single Algorithm Mode

1. Choose a scheduling strategy from the algorithm list.
2. Define the process workload with arrival time, burst time, and priority.
3. Set the quantum when the selected algorithm requires it.
4. Run the simulation.
5. Review the Gantt chart and metrics.

### Comparison Mode

1. Switch to the comparison workflow.
2. Select two algorithms.
3. Enter the same process dataset for both.
4. Run the comparison.
5. Review the average waiting time and other relevant metrics for each algorithm.

The application is designed to compare each algorithm against the same process set to highlight differences in scheduling behavior.

## Testing

The repository includes a Node-based verification suite in `tests/scheduler.test.mjs`. The tests validate:

- Correct algorithm IDs and total count
- FCFS, SJF, SRTF, RR, Priority, and RR+Priority behavior
- Idle-time handling
- Stability of input ordering
- Validation for invalid process data
- Metric and Gantt-chart consistency

Run the tests using:

```bash
node --test tests/scheduler.test.mjs
```

## Known Limitations

- The project is built as a client-side simulator and does not persist data.
- There is no backend service, authentication flow, or database.
- The application is intended for educational and visualization purposes rather than production scheduling.
- The original C implementation is included as a reference but is not the active runtime for the browser app.

## License

This repository does not currently include a license file. licence under SHOJIB-80.

## Author

MD Sirajul Islam
