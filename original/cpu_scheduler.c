/*=============================================================
   CPU SCHEDULING SIMULATOR  (Beginner-friendly, OS Lab project)
   ---------------------------------------------------------------
   Formulas used:
      TAT = CT - AT
      WT  = TAT - BT
      RT  = (time process first got CPU) - AT
   Algorithms: FCFS, SJF(NP), SRTF, Round Robin, Priority(NP),
               Round Robin + Priority
   Graphics: graphics.h / WinBGIm is used ONLY to draw the Gantt
             chart. Everything else is plain C (arrays + structs).
   =============================================================*/

#include <stdio.h>
#include <string.h>
#include <graphics.h>     /* WinBGIm graphics library */

#define MAX 10            /* maximum number of processes  */
#define MAXBAR 100        /* maximum gantt chart bars      */

/* ---------- one process ---------- */
struct Process {
    int pid;              /* process id entered by user       */
    int at, bt, pr;        /* arrival time, burst time, priority*/
    int rem;               /* remaining burst time (working)    */
    int ct, tat, wt, rt;   /* completion/turnaround/wait/response*/
    int started;           /* 1 once it has run at least once   */
};

/* ---------- one bar of the gantt chart ---------- */
struct Bar {
    int pid;        /* -1 means CPU is IDLE */
    int start, end;
};

/* ---------------- global data ---------------- */
struct Process porig[MAX];   /* original process data (never changed) */
struct Process p[MAX];       /* working copy used by each algorithm    */
int n;                        /* number of processes                    */
int quantum;                  /* time quantum for Round Robin            */

struct Bar gc[MAXBAR];        /* gantt chart bars for the last run       */
int gn;                        /* how many bars are filled                */

/* =========================================================
   Reset the working copy before running a new algorithm
   =========================================================*/
void reset(void) {
    int i;
    for (i = 0; i < n; i++) {
        p[i] = porig[i];
        p[i].rem = p[i].bt;
        p[i].ct = 0; p[i].tat = 0; p[i].wt = 0; p[i].rt = 0;
        p[i].started = 0;
    }
    gn = 0;
}

/* add one bar to the gantt chart array */
void addBar(int pid, int start, int end) {
    if (start == end) return;               /* skip empty bars */
    gc[gn].pid = pid;
    gc[gn].start = start;
    gc[gn].end = end;
    gn++;
}

/* find arrival time of the process that arrives earliest among
   those not yet finished (used to jump over an idle CPU gap)   */
int nextArrival(void) {
    int i, best = 999999;
    for (i = 0; i < n; i++)
        if (p[i].rem > 0 && p[i].at < best) best = p[i].at;
    return best;
}

/* =========================================================
   1) FCFS  -  First Come First Serve
   =========================================================*/
void fcfs(void) {
    int time = 0, i, j, done = 0;
    int order[MAX];
    for (i = 0; i < n; i++) order[i] = i;

    /* simple bubble sort by arrival time */
    for (i = 0; i < n - 1; i++)
        for (j = 0; j < n - 1 - i; j++)
            if (p[order[j]].at > p[order[j + 1]].at) {
                int t = order[j]; order[j] = order[j + 1]; order[j + 1] = t;
            }

    for (i = 0; i < n; i++) {
        int idx = order[i];
        if (time < p[idx].at) {              /* CPU idle until this process arrives */
            addBar(-1, time, p[idx].at);
            time = p[idx].at;
        }
        p[idx].rt = time - p[idx].at;         /* first (and only) start */
        addBar(p[idx].pid, time, time + p[idx].bt);
        time = time + p[idx].bt;
        p[idx].rem = 0;
        p[idx].ct = time;
        p[idx].tat = p[idx].ct - p[idx].at;
        p[idx].wt = p[idx].tat - p[idx].bt;
        done++;
    }
}

/* =========================================================
   2) SJF (Non-Preemptive) - shortest burst time first
   =========================================================*/
void sjf(void) {
    int time = 0, done = 0, i;

    while (done < n) {
        int best = -1;
        for (i = 0; i < n; i++) {
            if (p[i].rem > 0 && p[i].at <= time) {
                if (best == -1 || p[i].bt < p[best].bt)
                    best = i;
            }
        }
        if (best == -1) {                      /* nobody has arrived: CPU idle */
            int nt = nextArrival();
            addBar(-1, time, nt);
            time = nt;
            continue;
        }
        p[best].rt = time - p[best].at;
        addBar(p[best].pid, time, time + p[best].bt);
        time = time + p[best].bt;
        p[best].rem = 0;
        p[best].ct = time;
        p[best].tat = p[best].ct - p[best].at;
        p[best].wt = p[best].tat - p[best].bt;
        done++;
    }
}

/* =========================================================
   3) SRTF - Shortest Remaining Time First (Preemptive)
   =========================================================*/
void srtf(void) {
    int time = 0, done = 0, i;

    while (done < n) {
        int best = -1;
        for (i = 0; i < n; i++) {
            if (p[i].rem > 0 && p[i].at <= time) {
                if (best == -1 || p[i].rem < p[best].rem)
                    best = i;
            }
        }
        if (best == -1) {                      /* CPU idle */
            int nt = nextArrival();
            addBar(-1, time, nt);
            time = nt;
            continue;
        }
        if (p[best].started == 0) {
            p[best].started = 1;
            p[best].rt = time - p[best].at;
        }
        /* run the chosen process for just ONE time unit, then re-check  */
        if (gn > 0 && gc[gn - 1].pid == p[best].pid && gc[gn - 1].end == time)
            gc[gn - 1].end = time + 1;         /* extend the same bar */
        else
            addBar(p[best].pid, time, time + 1);

        p[best].rem--;
        time++;
        if (p[best].rem == 0) {
            p[best].ct = time;
            p[best].tat = p[best].ct - p[best].at;
            p[best].wt = p[best].tat - p[best].bt;
            done++;
        }
    }
}

/* =========================================================
   4) ROUND ROBIN
   =========================================================*/
void rr(void) {
    int queue[50], front = 0, rear = 0;
    int inQueue[MAX] = {0};
    int time = 0, done = 0, i;

    /* put every process that has arrived at time 0 into the queue */
    for (i = 0; i < n; i++)
        if (p[i].at <= time && !inQueue[i]) { queue[rear++] = i; inQueue[i] = 1; }

    while (done < n) {
        if (front == rear) {                   /* queue empty -> CPU idle */
            int nt = nextArrival();
            addBar(-1, time, nt);
            time = nt;
            for (i = 0; i < n; i++)
                if (p[i].at <= time && !inQueue[i]) { queue[rear++] = i; inQueue[i] = 1; }
            continue;
        }
        int cur = queue[front++];              /* take process from front */

        if (p[cur].started == 0) {
            p[cur].started = 1;
            p[cur].rt = time - p[cur].at;
        }

        int slice = (p[cur].rem < quantum) ? p[cur].rem : quantum;
        addBar(p[cur].pid, time, time + slice);
        time = time + slice;
        p[cur].rem = p[cur].rem - slice;

        /* processes that arrived during this slice join the queue
           BEFORE we put the current process back (standard RR rule) */
        for (i = 0; i < n; i++)
            if (p[i].at <= time && !inQueue[i]) { queue[rear++] = i; inQueue[i] = 1; }

        if (p[cur].rem == 0) {
            p[cur].ct = time;
            p[cur].tat = p[cur].ct - p[cur].at;
            p[cur].wt = p[cur].tat - p[cur].bt;
            done++;
        } else {
            queue[rear++] = cur;               /* not finished: back of queue */
        }
    }
}

/* =========================================================
   5) PRIORITY SCHEDULING (Non-Preemptive) - lower number = higher priority
   =========================================================*/
void priority(void) {
    int time = 0, done = 0, i;

    while (done < n) {
        int best = -1;
        for (i = 0; i < n; i++) {
            if (p[i].rem > 0 && p[i].at <= time) {
                if (best == -1 || p[i].pr < p[best].pr)
                    best = i;
            }
        }
        if (best == -1) {
            int nt = nextArrival();
            addBar(-1, time, nt);
            time = nt;
            continue;
        }
        p[best].rt = time - p[best].at;
        addBar(p[best].pid, time, time + p[best].bt);
        time = time + p[best].bt;
        p[best].rem = 0;
        p[best].ct = time;
        p[best].tat = p[best].ct - p[best].at;
        p[best].wt = p[best].tat - p[best].bt;
        done++;
    }
}

/* =========================================================
   6) ROUND ROBIN + PRIORITY
   Same as Round Robin, but every time we pick the next process
   we choose the HIGHEST PRIORITY process among those currently
   waiting in the queue (instead of always taking the front).
   =========================================================*/
void rrPriority(void) {
    int queue[50], count = 0;
    int inQueue[MAX] = {0};
    int time = 0, done = 0, i, j;

    for (i = 0; i < n; i++)
        if (p[i].at <= time && !inQueue[i]) { queue[count++] = i; inQueue[i] = 1; }

    while (done < n) {
        if (count == 0) {                      /* queue empty -> CPU idle */
            int nt = nextArrival();
            addBar(-1, time, nt);
            time = nt;
            for (i = 0; i < n; i++)
                if (p[i].at <= time && !inQueue[i]) { queue[count++] = i; inQueue[i] = 1; }
            continue;
        }

        /* pick the highest priority (smallest pr) process in the queue */
        int pick = 0;
        for (i = 1; i < count; i++)
            if (p[queue[i]].pr < p[queue[pick]].pr) pick = i;

        int cur = queue[pick];
        /* remove it from the queue by shifting left */
        for (j = pick; j < count - 1; j++) queue[j] = queue[j + 1];
        count--;

        if (p[cur].started == 0) {
            p[cur].started = 1;
            p[cur].rt = time - p[cur].at;
        }

        int slice = (p[cur].rem < quantum) ? p[cur].rem : quantum;
        addBar(p[cur].pid, time, time + slice);
        time = time + slice;
        p[cur].rem = p[cur].rem - slice;

        for (i = 0; i < n; i++)
            if (p[i].at <= time && !inQueue[i]) { queue[count++] = i; inQueue[i] = 1; }

        if (p[cur].rem == 0) {
            p[cur].ct = time;
            p[cur].tat = p[cur].ct - p[cur].at;
            p[cur].wt = p[cur].tat - p[cur].bt;
            done++;
        } else {
            queue[count++] = cur;
        }
    }
}

/* =========================================================
   Run any algorithm by number (used by menu AND by compare)
   1=FCFS 2=SJF 3=SRTF 4=RR 5=Priority 6=RR+Priority
   =========================================================*/
void runAlgo(int choice) {
    if (choice == 1) fcfs();
    else if (choice == 2) sjf();
    else if (choice == 3) srtf();
    else if (choice == 4) rr();
    else if (choice == 5) priority();
    else if (choice == 6) rrPriority();
}

char *algoName(int choice) {
    if (choice == 1) return "FCFS";
    if (choice == 2) return "SJF (Non-Preemptive)";
    if (choice == 3) return "SRTF (Preemptive)";
    if (choice == 4) return "Round Robin";
    if (choice == 5) return "Priority Scheduling";
    if (choice == 6) return "Round Robin + Priority";
    return "Unknown";
}

/* average waiting time of the CURRENT p[] array */
double avgWT(void) {
    int i; double sum = 0;
    for (i = 0; i < n; i++) sum = sum + p[i].wt;
    return sum / n;
}

/* =========================================================
   Print the result table + averages
   =========================================================*/
void showTable(char *name) {
    int i;
    double swt = 0, stat = 0, srt = 0;

    printf("\n---------------------------------------------------\n");
    printf(" %s - RESULTS\n", name);
    printf("---------------------------------------------------\n");
    printf("PID\tAT\tBT\tPR\tCT\tTAT\tWT\tRT\n");
    for (i = 0; i < n; i++) {
        printf("P%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
            p[i].pid, p[i].at, p[i].bt, p[i].pr,
            p[i].ct, p[i].tat, p[i].wt, p[i].rt);
        swt += p[i].wt; stat += p[i].tat; srt += p[i].rt;
    }
    printf("---------------------------------------------------\n");
    printf("Average Waiting Time    = %.2f\n", swt / n);
    printf("Average Turnaround Time = %.2f\n", stat / n);
    printf("Average Response Time   = %.2f\n", srt / n);
}

/* =========================================================
   Draw the Gantt chart using graphics.h  (only place graphics
   functions are used in the whole program)
   =========================================================*/
void drawGantt(char *title) {
    int i;
    int totalTime = gc[gn - 1].end;
    int x = 40, y = 200, height = 60, width = 720;
    double scale = (double) width / totalTime;
    int colors[7] = {LIGHTRED, YELLOW, GREEN, CYAN, LIGHTMAGENTA, LIGHTBLUE, BROWN};

    initwindow(800, 350, title);
    setbkcolor(BLACK);

    setcolor(WHITE);
    outtextxy(40, 30, title);

    for (i = 0; i < gn; i++) {
        int x1 = x + (int)(gc[i].start * scale);
        int x2 = x + (int)(gc[i].end   * scale);
        char label[10], num[10];

        if (gc[i].pid == -1) {                 /* IDLE block: grey */
            setfillstyle(SOLID_FILL, DARKGRAY);
            strcpy(label, "IDLE");
        } else {                                /* pick a color by pid */
            setfillstyle(SOLID_FILL, colors[gc[i].pid % 7]);
            sprintf(label, "P%d", gc[i].pid);
        }
        bar(x1, y, x2, y + height);
        setcolor(WHITE);
        rectangle(x1, y, x2, y + height);
        outtextxy((x1 + x2) / 2 - 8, y + height / 2 - 8, label);

        sprintf(num, "%d", gc[i].start);
        outtextxy(x1 - 4, y + height + 10, num);
    }
    char last[10];
    sprintf(last, "%d", totalTime);
    outtextxy(x + width - 4, y + height + 10, last);

    outtextxy(40, 300, "Press any key to close this window...");
    getch();
    closegraph();
}

/* =========================================================
   Take process information from the user
   =========================================================*/
void inputProcesses(void) {
    int i;
    printf("Enter number of processes (max %d): ", MAX);
    scanf("%d", &n);
    if (n < 1) n = 1;
    if (n > MAX) n = MAX;

    printf("Enter time quantum (for Round Robin): ");
    scanf("%d", &quantum);
    if (quantum < 1) quantum = 1;

    for (i = 0; i < n; i++) {
        printf("\nProcess %d:\n", i + 1);
        printf("  Process ID (number)                : "); scanf("%d", &porig[i].pid);
        printf("  Arrival Time                       : "); scanf("%d", &porig[i].at);
        printf("  Burst Time                         : "); scanf("%d", &porig[i].bt);
        printf("  Priority (lower number = higher)   : "); scanf("%d", &porig[i].pr);
    }
    printf("\nProcess data saved!\n");
}

void viewProcesses(void) {
    int i;
    if (n == 0) { printf("\nNo data yet. Choose option 1 first.\n"); return; }
    printf("\nPID\tAT\tBT\tPriority\n");
    for (i = 0; i < n; i++)
        printf("P%d\t%d\t%d\t%d\n", porig[i].pid, porig[i].at, porig[i].bt, porig[i].pr);
    printf("Time Quantum = %d\n", quantum);
}

/* =========================================================
   Run one algorithm, show its table, and draw its Gantt chart
   =========================================================*/
void showAlgorithm(int choice) {
    if (n == 0) { printf("\nPlease enter process information first (Option 1).\n"); return; }
    reset();
    runAlgo(choice);
    showTable(algoName(choice));
    drawGantt(algoName(choice));
}

/* =========================================================
   Compare two algorithms using Average Waiting Time
   =========================================================*/
void compareAlgorithms(void) {
    int c1, c2;
    double wt1, wt2;

    if (n == 0) { printf("\nPlease enter process information first (Option 1).\n"); return; }

    printf("\n1.FCFS 2.SJF 3.SRTF 4.RoundRobin 5.Priority 6.RR+Priority\n");
    printf("Enter first algorithm choice : "); scanf("%d", &c1);
    printf("Enter second algorithm choice: "); scanf("%d", &c2);

    reset(); runAlgo(c1); wt1 = avgWT();
    reset(); runAlgo(c2); wt2 = avgWT();

    printf("\n---------------------------------------------------\n");
    printf(" COMPARISON (Average Waiting Time)\n");
    printf("---------------------------------------------------\n");
    printf("%-25s : %.2f\n", algoName(c1), wt1);
    printf("%-25s : %.2f\n", algoName(c2), wt2);
    printf("---------------------------------------------------\n");

    if (wt1 < wt2)
        printf("BEST ALGORITHM: %s (lower average waiting time)\n", algoName(c1));
    else if (wt2 < wt1)
        printf("BEST ALGORITHM: %s (lower average waiting time)\n", algoName(c2));
    else
        printf("Both algorithms give the SAME average waiting time.\n");
}

/* =========================================================
   MAIN MENU
   =========================================================*/
int main(void) {
    int choice;

    do {
        printf("\n=====================================================\n");
        printf("        CPU SCHEDULING ALGORITHM SIMULATOR\n");
        printf("=====================================================\n");
        printf("1. Enter Process Information\n");
        printf("2. FCFS\n");
        printf("3. SJF (Non-Preemptive)\n");
        printf("4. SRTF (Preemptive)\n");
        printf("5. Round Robin\n");
        printf("6. Priority Scheduling\n");
        printf("7. Round Robin + Priority\n");
        printf("8. Compare Two Algorithms\n");
        printf("9. View Process Information\n");
        printf("0. Exit\n");
        printf("-----------------------------------------------------\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: inputProcesses(); break;
            case 2: showAlgorithm(1); break;
            case 3: showAlgorithm(2); break;
            case 4: showAlgorithm(3); break;
            case 5: showAlgorithm(4); break;
            case 6: showAlgorithm(5); break;
            case 7: showAlgorithm(6); break;
            case 8: compareAlgorithms(); break;
            case 9: viewProcesses(); break;
            case 0: printf("\nExiting... Thank you!\n"); break;
            default: printf("\nInvalid choice, try again.\n");
        }
    } while (choice != 0);

    return 0;
}