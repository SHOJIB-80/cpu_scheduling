#include <stdio.h>
#include <stdlib.h>
#define MAXP 50
#define MAXG 5000

typedef struct {
    int pid;
    int arrival;
    int burst;
    int priority;
    int waiting;
    int turnaround;
    int completion;
    int remaining;
} Process;

void printLine(void) {
    printf("------------------------------------------------------------\n");
}

void inputProcesses(Process p[], int *n) {
    int i;

    printf("Enter number of processes: ");
    scanf("%d", n);

    if (*n <= 0 || *n > MAXP) {
        printf("Invalid number of processes!\n");
        exit(0);
    }

    for (i = 0; i < *n; i++) {
        p[i].pid = i + 1;

        printf("\nProcess %d burst time: ", i + 1);
        scanf("%d", &p[i].burst);

        while (p[i].burst <= 0) {
            printf("Burst time must be greater than 0. Enter again: ");
            scanf("%d", &p[i].burst);
        }

        printf("Process %d arrival time: ", i + 1);
        scanf("%d", &p[i].arrival);

        while (p[i].arrival < 0) {
            printf("Arrival time cannot be negative. Enter again: ");
            scanf("%d", &p[i].arrival);
        }

        printf("Process %d priority (lower number = higher priority): ", i + 1);
        scanf("%d", &p[i].priority);

        p[i].waiting = 0;
        p[i].turnaround = 0;
        p[i].completion = 0;
        p[i].remaining = p[i].burst;
    }
}

void copyProcesses(Process source[], Process dest[], int n) {
    int i;
    for (i = 0; i < n; i++)
        dest[i] = source[i];
}

void addSegment(int order[], int start[], int end[], int *count,
                int pid, int s, int e) {
    if (s == e)
        return;

    if (*count > 0 && order[*count - 1] == pid &&
        end[*count - 1] == s) {
        end[*count - 1] = e;
    } else if (*count < MAXG) {
        order[*count] = pid;
        start[*count] = s;
        end[*count] = e;
        (*count)++;
    }
}

void printGantt(int order[], int start[], int end[], int count) {
    int i;

    printf("\nGantt Chart:\n");
    for (i = 0; i < count; i++) {
        if (order[i] == 0)
            printf("| IDLE  ");
        else
            printf("| P%-5d ", order[i]);
    }
    printf("|\n");

    printf("%d", start[0]);
    for (i = 0; i < count; i++)
        printf("%9d", end[i]);
    printf("\n");
}


float showResults(Process p[], int n, const char *name, int showPriority,
                  int tq, int order[], int start[], int end[], int count) {
    int i;
    float totalWait = 0, totalTurn = 0;

    printLine();
    if (tq > 0)
        printf("%s (Time Quantum = %d)\n", name, tq);
    else
        printf("%s\n", name);
    printLine();

    printGantt(order, start, end, count);

    if (showPriority)
        printf("\nPID  Arr  Burst  Pri  Comp  Wait  Turn\n");
    else
        printf("\nPID  Arr  Burst  Comp  Wait  Turn\n");

    for (i = 0; i < n; i++) {
        if (showPriority)
            printf("%-4d %-4d %-6d %-4d %-5d %-5d %-5d\n",
                   p[i].pid, p[i].arrival, p[i].burst, p[i].priority,
                   p[i].completion, p[i].waiting, p[i].turnaround);
        else
            printf("%-4d %-4d %-6d %-5d %-5d %-5d\n",
                   p[i].pid, p[i].arrival, p[i].burst,
                   p[i].completion, p[i].waiting, p[i].turnaround);

        totalWait += p[i].waiting;
        totalTurn += p[i].turnaround;
    }

    printf("\nAverage Waiting Time: %.2f\n", totalWait / n);
    printf("Average Turnaround Time: %.2f\n", totalTurn / n);

    return totalWait / n;
}

/* ---------- FCFS ---------- */

float fcfs(Process p[], int n) {
    int i, j, time = 0, count = 0;
    Process temp;
    int order[MAXG], start[MAXG], end[MAXG];

    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - 1 - i; j++) {
            if (p[j].arrival > p[j + 1].arrival) {
                temp = p[j];
                p[j] = p[j + 1];
                p[j + 1] = temp;
            }
        }
    }

    for (i = 0; i < n; i++) {
        if (time < p[i].arrival) {
            addSegment(order, start, end, &count, 0, time, p[i].arrival);
            time = p[i].arrival;
        }

        p[i].waiting = time - p[i].arrival;
        addSegment(order, start, end, &count, p[i].pid,
                   time, time + p[i].burst);

        time += p[i].burst;
        p[i].completion = time;
        p[i].turnaround = p[i].completion - p[i].arrival;
    }

    return showResults(p, n, "FCFS Results", 0, 0,
                       order, start, end, count);
}

/* ---------- SJF Non-Preemptive ---------- */

float sjf(Process p[], int n) {
    int completed = 0, time = 0, count = 0;
    int used[MAXP] = {0};
    int order[MAXG], start[MAXG], end[MAXG];

    while (completed < n) {
        int i, idx = -1;

        for (i = 0; i < n; i++) {
            if (!used[i] && p[i].arrival <= time) {
                if (idx == -1 ||
                    p[i].burst < p[idx].burst ||
                    (p[i].burst == p[idx].burst && p[i].pid < p[idx].pid)) {
                    idx = i;
                }
            }
        }

        if (idx == -1) {
            int next = -1;

            for (i = 0; i < n; i++) {
                if (!used[i] && (next == -1 || p[i].arrival < p[next].arrival))
                    next = i;
            }

            addSegment(order, start, end, &count, 0, time, p[next].arrival);
            time = p[next].arrival;
            continue;
        }

        p[idx].waiting = time - p[idx].arrival;

        addSegment(order, start, end, &count, p[idx].pid,
                   time, time + p[idx].burst);

        time += p[idx].burst;
        p[idx].completion = time;
        p[idx].turnaround = time - p[idx].arrival;
        used[idx] = 1;
        completed++;
    }

    return showResults(p, n, "SJF (Non-Preemptive) Results", 0, 0,
                       order, start, end, count);
}

/* ---------- SRTF ---------- */

float srtf(Process p[], int n) {
    int completed = 0, time = 0, count = 0;
    int order[MAXG], start[MAXG], end[MAXG];

    while (completed < n) {
        int i, idx = -1;

        for (i = 0; i < n; i++) {
            if (p[i].arrival <= time && p[i].remaining > 0) {
                if (idx == -1 ||
                    p[i].remaining < p[idx].remaining ||
                    (p[i].remaining == p[idx].remaining &&
                     p[i].arrival < p[idx].arrival) ||
                    (p[i].remaining == p[idx].remaining &&
                     p[i].arrival == p[idx].arrival &&
                     p[i].pid < p[idx].pid)) {
                    idx = i;
                }
            }
        }

        if (idx == -1) {
            int next = -1;

            for (i = 0; i < n; i++) {
                if (p[i].remaining > 0 &&
                    (next == -1 || p[i].arrival < p[next].arrival))
                    next = i;
            }

            addSegment(order, start, end, &count, 0, time, p[next].arrival);
            time = p[next].arrival;
            continue;
        }

        addSegment(order, start, end, &count, p[idx].pid, time, time + 1);

        p[idx].remaining--;
        time++;

        if (p[idx].remaining == 0) {
            p[idx].completion = time;
            p[idx].turnaround = time - p[idx].arrival;
            p[idx].waiting = p[idx].turnaround - p[idx].burst;
            completed++;
        }
    }

    return showResults(p, n, "SRTF (Preemptive) Results", 0, 0,
                       order, start, end, count);
}

/* ---------- Priority Non-Preemptive ---------- */

float priorityScheduling(Process p[], int n) {
    int completed = 0, time = 0, count = 0;
    int used[MAXP] = {0};
    int order[MAXG], start[MAXG], end[MAXG];

    while (completed < n) {
        int i, idx = -1;

        for (i = 0; i < n; i++) {
            if (!used[i] && p[i].arrival <= time) {
                if (idx == -1 ||
                    p[i].priority < p[idx].priority ||
                    (p[i].priority == p[idx].priority &&
                     p[i].pid < p[idx].pid)) {
                    idx = i;
                }
            }
        }

        if (idx == -1) {
            int next = -1;

            for (i = 0; i < n; i++) {
                if (!used[i] &&
                    (next == -1 || p[i].arrival < p[next].arrival))
                    next = i;
            }

            addSegment(order, start, end, &count, 0, time, p[next].arrival);
            time = p[next].arrival;
            continue;
        }

        p[idx].waiting = time - p[idx].arrival;

        addSegment(order, start, end, &count, p[idx].pid,
                   time, time + p[idx].burst);

        time += p[idx].burst;
        p[idx].completion = time;
        p[idx].turnaround = time - p[idx].arrival;

        used[idx] = 1;
        completed++;
    }

    return showResults(p, n, "Priority (Non-Preemptive) Results", 1, 0,
                       order, start, end, count);
}

/* ---------- Round Robin ---------- */

float roundRobin(Process p[], int n, int tq) {
    int time = 0, completed = 0, count = 0;
    int order[MAXG], start[MAXG], end[MAXG];
    int added[MAXP] = {0};
    int queue[MAXP];
    int front = 0, rear = 0, qcount = 0;

    while (completed < n) {
        int i, idx, run;

        for (i = 0; i < n; i++) {
            if (!added[i] && p[i].arrival <= time) {
                queue[rear] = i;
                rear = (rear + 1) % MAXP;
                qcount++;
                added[i] = 1;
            }
        }

        if (qcount == 0) {
            int next = -1;

            for (i = 0; i < n; i++) {
                if (p[i].remaining > 0 &&
                    (next == -1 || p[i].arrival < p[next].arrival))
                    next = i;
            }

            addSegment(order, start, end, &count, 0, time, p[next].arrival);
            time = p[next].arrival;
            continue;
        }

        idx = queue[front];
        front = (front + 1) % MAXP;
        qcount--;

        run = p[idx].remaining < tq ? p[idx].remaining : tq;

        addSegment(order, start, end, &count, p[idx].pid,
                   time, time + run);

        time += run;
        p[idx].remaining -= run;

        for (i = 0; i < n; i++) {
            if (!added[i] && p[i].arrival <= time) {
                queue[rear] = i;
                rear = (rear + 1) % MAXP;
                qcount++;
                added[i] = 1;
            }
        }

        if (p[idx].remaining > 0) {
            queue[rear] = idx;
            rear = (rear + 1) % MAXP;
            qcount++;
        } else {
            p[idx].completion = time;
            p[idx].turnaround = time - p[idx].arrival;
            p[idx].waiting = p[idx].turnaround - p[idx].burst;
            completed++;
        }
    }

    return showResults(p, n, "Round Robin Results", 0, tq,
                       order, start, end, count);
}

/* ---------- Round Robin with Priority ---------- */


float priorityRoundRobin(Process p[], int n, int tq) {
    int time = 0, completed = 0, count = 0;
    int order[MAXG], start[MAXG], end[MAXG];
    int lastIndex = -1;

    while (completed < n) {
        int i, idx = -1;
        int bestPriority = 1000000;

        for (i = 0; i < n; i++) {
            if (p[i].remaining > 0 && p[i].arrival <= time) {
                if (p[i].priority < bestPriority) {
                    bestPriority = p[i].priority;
                    idx = i;
                }
            }
        }

        if (idx != -1) {
            for (i = 1; i <= n; i++) {
                int pos = (lastIndex + i) % n;

                if (p[pos].remaining > 0 &&
                    p[pos].arrival <= time &&
                    p[pos].priority == bestPriority) {
                    idx = pos;
                    break;
                }
            }
        }

        if (idx == -1) {
            int next = -1;

            for (i = 0; i < n; i++) {
                if (p[i].remaining > 0 &&
                    (next == -1 || p[i].arrival < p[next].arrival))
                    next = i;
            }

            addSegment(order, start, end, &count, 0, time, p[next].arrival);
            time = p[next].arrival;
            continue;
        }

        {
            int run = p[idx].remaining < tq ? p[idx].remaining : tq;

            addSegment(order, start, end, &count, p[idx].pid,
                       time, time + run);

            time += run;
            p[idx].remaining -= run;
            lastIndex = idx;

            if (p[idx].remaining == 0) {
                p[idx].completion = time;
                p[idx].turnaround = time - p[idx].arrival;
                p[idx].waiting = p[idx].turnaround - p[idx].burst;
                completed++;
            }
        }
    }

    return showResults(p, n, "Round Robin with Priority Results",
                       1, tq, order, start, end, count);
}

/* ---------- Algorithm runner ---------- */

float runAlgorithm(int choice, Process original[], int n, int tq) {
    Process p[MAXP];
    copyProcesses(original, p, n);

    switch (choice) {
        case 1: return fcfs(p, n);
        case 2: return sjf(p, n);
        case 3: return srtf(p, n);
        case 4: return roundRobin(p, n, tq);
        case 5: return priorityScheduling(p, n);
        case 6: return priorityRoundRobin(p, n, tq);
        default: return 0;
    }
}

void algorithmName(int choice) {
    switch (choice) {
        case 1: printf("FCFS"); break;
        case 2: printf("SJF"); break;
        case 3: printf("SRTF"); break;
        case 4: printf("Round Robin"); break;
        case 5: printf("Priority"); break;
        case 6: printf("Round Robin with Priority"); break;
    }
}

int getAlgorithmChoice(void) {
    int choice;

    printf("\n1. FCFS\n");
    printf("2. SJF\n");
    printf("3. SRTF\n");
    printf("4. Round Robin\n");
    printf("5. Priority Scheduling\n");
    printf("6. Round Robin with Priority\n");
    printf("Enter algorithm: ");
    scanf("%d", &choice);

    if (choice < 1 || choice > 6)
        return -1;

    return choice;
}

/* ---------- Main ---------- */

int main(void) {
    Process p[MAXP];
    int n, choice, tq;

    while (1) {
        printLine();
        printf("             CPU SCHEDULING SIMULATOR\n");
        printLine();
        printf("1. FCFS\n");
        printf("2. SJF (Non-Preemptive)\n");
        printf("3. SRTF (Preemptive)\n");
        printf("4. Round Robin\n");
        printf("5. Priority Scheduling\n");
        printf("6. Round Robin with Priority\n");
        printf("7. Compare Two Algorithms\n");
        printf("8. Exit\n");
        printLine();
        printf("Enter your choice: ");
        scanf("%d", &choice);

        if (choice == 8) {
            printf("Exiting...\n");
            break;
        }

        if (choice < 1 || choice > 8) {
            printf("Invalid choice!\n");
            continue;
        }

        inputProcesses(p, &n);

        if (choice == 4 || choice == 6) {
            printf("\nEnter Time Quantum: ");
            scanf("%d", &tq);

            if (tq <= 0) {
                printf("Invalid Time Quantum!\n");
                continue;
            }

            runAlgorithm(choice, p, n, tq);
        }
        else if (choice >= 1 && choice <= 6) {
            runAlgorithm(choice, p, n, 0);
        }
        else if (choice == 7) {
            int a1, a2;
            int needTQ = 0;

            printf("\nSelect First Algorithm:\n");
            a1 = getAlgorithmChoice();

            printf("\nSelect Second Algorithm:\n");
            a2 = getAlgorithmChoice();

            if (a1 == -1 || a2 == -1) {
                printf("Invalid algorithm choice!\n");
                continue;
            }

            if (a1 == 4 || a1 == 6 || a2 == 4 || a2 == 6)
                needTQ = 1;

            tq = 0;

            if (needTQ) {
                printf("\nEnter Time Quantum: ");
                scanf("%d", &tq);

                if (tq <= 0) {
                    printf("Invalid Time Quantum!\n");
                    continue;
                }
            }

            printf("\n\n================ ALGORITHM 1 ================\n");
            algorithmName(a1);
            printf("\n");
            {
                float avg1 = runAlgorithm(a1, p, n, tq);

                printf("\n\n================ ALGORITHM 2 ================\n");
                algorithmName(a2);
                printf("\n");
                {
                    float avg2 = runAlgorithm(a2, p, n, tq);

                    printLine();
                    printf("                 COMPARISON\n");
                    printLine();

                    algorithmName(a1);
                    printf(" Average Waiting Time: %.2f\n", avg1);

                    algorithmName(a2);
                    printf(" Average Waiting Time: %.2f\n", avg2);

                    if (avg1 < avg2) {
                        printf("\nBest Algorithm: ");
                        algorithmName(a1);
                        printf("\nReason: Lower Average Waiting Time.\n");
                    }
                    else if (avg2 < avg1) {
                        printf("\nBest Algorithm: ");
                        algorithmName(a2);
                        printf("\nReason: Lower Average Waiting Time.\n");
                    }
                    else {
                        printf("\nBoth algorithms have the same Average Waiting Time.\n");
                    }

                    printLine();
                }
            }
        }

        printf("\n");
    }

    return 0;
}
