# System Flows
This document explicitly maps out the step-by-step logic for how the Object-Oriented components interact, as well as how the Sequential algorithms work versus how the OpenMP Parallel algorithms handle physical memory architecture.

## 0. System Component Interaction Flow
This flowchart maps exactly how the classes defined in `class_architecture.md` interact with each other during a user's session:

```mermaid
graph TD
    Main[main.cpp] --> DL[DataLoader]
    DL -->|Loads CSVs into RAM| DB[(DatabaseEngine)]
    Main --> CLI[CLIController]
    CLI -->|Sends Query Request| QO[QueryOptimizer]
    QO -.->|Fetches arrays| DB
    QO -->|1. Executes Baseline| SA[SerialAlgorithms]
    QO -->|2. Executes OpenMP| PA[ParallelAlgorithms]
    SA -.->|Returns Time| QO
    PA -.->|Returns Time| QO
    QO -->|3. Calculates Speedup| CLI
    CLI -->|Prints Graph| User((User))
```

---

## 1. Flow of Aggregation (`SUM`, `AVG`)

```mermaid
graph TD
    A[Global array of Payrolls] -->|Chunk to threads| B
    B[OpenMP Parallel Execution] --> T1[Thread 1]
    B --> T2[Thread 2]
    B --> T3[Thread 3]
    T1 -->|Sums idx 0-20M| L1[Thread-Local Sum 1]
    T2 -->|Sums idx 20M-40M| L2[Thread-Local Sum 2]
    T3 -->|Sums idx 40M-60M| L3[Thread-Local Sum 3]
    L1 -->|Reduces into| G[(Final Global Sum)]
    L2 -->|Reduces into| G
    L3 -->|Reduces into| G
```

### The Parallel Flow
1.  **Initialize Variables:** Create a global `double total_sum = 0;`
2.  **Thread Setup:** The program hits `#pragma omp parallel for reduction(+:total_sum)`.
3.  **Array Chunking:** OpenMP divides the records into chunks.
4.  **Local Accumulation:** OpenMP gives every thread its own private, isolated copy of `total_sum` (Preventing Race Conditions).
5.  **Recombination (The Reduction Phase):** OpenMP safely adds the local sums together into the original global `total_sum`.

---

## 2. Flow of Filtering (`WHERE` clause)

```mermaid
graph TD
    Data[(10M Employees Array)] --> |#pragma omp parallel for| Threads
    Threads --> T1(Thread 1)
    Threads --> T2(Thread 2)
    T1 -->|Matches| V1[Thread-Local Vector]
    T2 -->|Matches| V2[Thread-Local Vector]
    V1 -->|#pragma omp critical| Final[Master Vector]
    V2 -->|#pragma omp critical| Final
```

### The Parallel Flow
1.  **Preparation:** OpenMP cannot safely do a `push_back()` to a single array from multiple threads.
2.  **Thread Local Storage:** Each thread creates its own isolated `std::vector` inside the parallel region.
3.  **Chunk Processing:** `#pragma omp for` divides the huge Employee array.
4.  **Global Combination:** After filtering, a `#pragma omp critical` block safely appends the contents of local arrays into the master `matching_ids`.

---

## 3. Flow of Database Hash Join

```mermaid
graph TD
    S(Departments Array) -->|Sequential Build| HM{{Read-Only Hash Map}}
    M(Employees Array) -->|#pragma omp parallel for| Probe
    Probe --> T1[Thread 1]
    Probe --> T2[Thread 2]
    HM -.->|Lock-free Lookup| T1
    HM -.->|Lock-free Lookup| T2
    T1 -->|Push_back| L1[Local Result Array]
    T2 -->|Push_back| L2[Local Result Array]
    L1 -->|Combine| G[(Global Joined Array)]
    L2 -->|Combine| G
```

### The Parallel Flow
1.  **Hash Phase:** Build hash map sequentially (it takes very little time).
2.  **Read-Only Broadcast:** The constructed Hash Map is locked to **Read-Only** mode.
3.  **Concurrent Probing:** `#pragma omp parallel for` across the `Employees` array. All threads can safely read the `Departments` map simultaneously. 
4.  **Merging:** Threads save results locally, combining them at the end.

---

## 4. Flow of Parallel Recursive Tasking (Merge Sort)

```mermaid
graph TD
    A[Unsorted Employee Array] -->|#pragma omp task| L1(Left Half)
    A -->|#pragma omp task| R1(Right Half)
    L1 -->|#pragma omp task| L2(Quarter 1)
    L1 -->|#pragma omp task| L3(Quarter 2)
    R1 --> R2(Quarter 3)
    R1 --> R3(Quarter 4)
    L2 -->|#pragma omp taskwait| C1{Merge 1}
    L3 -->|#pragma omp taskwait| C1
    C1 --> Final[(Sorted Array)]
```
### The Parallel Flow
1. The first thread encounters `#pragma omp single` to seed the root task.
2. The root task spawns two sub-tasks (`#pragma omp task`) for the left and right halves of the array.
3. Threads available in the pool pick up the tasks dynamically to sort sub-chunks.
4. A `#pragma omp taskwait` barrier blocks combining until both children finish.

---

## 5. Flow of Deep Temporal Filtering (Histograms)
Used for counting Absences and Multi-Table Map-Reducing (Group By).

```mermaid
graph TD
    D[(Millions of Logs)] -->|Parallel Chunk| P[Threads]
    P --> T1[Thread 1]
    P --> T2[Thread 2]
    T1 -->|Local Map| M1{{Hash Map: count++}}
    T2 -->|Local Map| M2{{Hash Map: count++}}
    M1 -.->|Global Loop| G{{Master Hash Map}}
    M2 -.->|Global Loop| G
```
### The Parallel Flow
1. We cannot use `reduction(+:sum)` because Hash Maps are dynamic.
2. OpenMP creates a matrix of hash-maps (one Map for each thread).
3. Threads populate their private Hash Maps without locking.
4. We exit the parallel region and a sequential loop safely aggregates the `8 thread` maps into `1 Master` map.

---

## 6. Multi-Phase Dependencies (Subqueries)

```mermaid
graph TD
    subgraph Wave 1: Reduction
    A[Array] -->|parallel sum| G(Global Avg)
    end
    G --> B[Implicit Memory Barrier]
    B --> C
    subgraph Wave 2: Filtering
    C[Array] --> |Read Global Avg| T1
    end
```
### The Parallel Flow
1. OpenMP fires an initial swarm of threads to Map-Reduce the "Average Performance Score".
2. Because threads exit a `#pragma omp parallel` block, an implicit memory barrier is hit.
3. The next `#pragma omp parallel` block opens. The threads are fully synchronized and correctly aware of the newly built average, avoiding race conditions.
