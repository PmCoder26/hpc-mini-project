# Technology Stack & Technical Details

This document outlines the specific technologies, tools, and technical algorithms used to implement the **HPC Enterprise HR & Payroll Analytics Engine**.

## 1. Primary Technology Stack

### Backend & API
*   **Core Execution Language:** Python 3
    *   *Why:* Python handles HTTP requests very efficiently with FastAPI, integrates seamlessly with data tools like Pandas/NumPy, and allows straightforward binding to C/CUDA via PyCUDA for high-performance computing without forcing the entire project to be written in C++.
*   **Web Framework:** FastAPI & Uvicorn
    *   *Why:* FastAPI is a modern, high-performance web framework for building APIs with Python based on standard Python type hints. It is incredibly fast, easy to learn, and fast to code, making it ideal for the backend of our analytics engine.
*   **Parallelization Framework:** PyCUDA (GPU / CUDA)
    *   *Why:* PyCUDA gives Python access to NVIDIA's CUDA parallel computation API. Instead of executing serial loops over millions of datasets, we compile C++ CUDA code on the fly and execute it on thousands of GPU cores simultaneously, providing massive speedup for data operations.
*   **Data Handling:** Pandas & NumPy
    *   *Why:* Pandas provides the core DataFrame structure for an in-memory SQL-like representation. NumPy is utilized to ensure that the arrays passed to PyCUDA are perfectly contiguous C-style structures in RAM.

### Frontend
*   **Structure:** HTML5 & Vanilla JavaScript
    *   *Why:* We rely on modern ES6+ JavaScript and DOM manipulation, eliminating the need for heavyweight frontend frameworks for this specific benchmarking dashboard. The `fetch` API is used to perform asynchronous calls to the backend.
*   **Styling:** Vanilla CSS3
    *   *Why:* A custom design system built with CSS variables to handle theming. It implements a modern Dark Glassmorphism design aesthetic with dynamic micro-animations.
*   **Data Visualization:** Chart.js
    *   *Why:* Real-time, reactive charts updating as benchmarking results stream in from the GPU.

## 2. Technical Implementation Details

### A. Memory Management
The backend engine reads CSV files directly into RAM via Pandas upon API startup, loading the files into global variables managed by the `DatabaseEngine`.
*   **Data Structures:** Data mapped natively from CSV lines to strongly-typed Pandas `Series` and contiguous PyCUDA `cuda.mem_alloc` arrays.
*   *Example Execution Context:* When the GPU is called, the NumPy arrays must be cloned up to the VRAM:
    ```python
    data_gpu = cuda.mem_alloc(arr.nbytes)
    cuda.memcpy_htod(data_gpu, arr)
    ```

### B. Parallel Algorithms (PyCUDA)
Instead of `#pragma omp parallel`, the engine compiles strings of C++ and executes them across blocks and grids.
*   **Parallel Compaction (The `WHERE` clause):**
    *   *Mechanism:* A custom kernel that loops using `blockIdx.x * blockDim.x + threadIdx.x` and utilizes `atomicAdd` to avoid data race conditions as threads identify records meeting the filter conditions.
*   **Serial Fallback:** 
    *   *Mechanism:* Simple Python `for` loops across elements to provide a measured baseline against which we graph the Speedup.

## 3. Benchmarking & Metrics Gathering

To prove the project is an HPC success, the `QueryOptimizer` intercepts API calls and processes them through an execution pipeline wrapper.

*   **Timing Library:** Python's `time.time()` or High Precision `time.perf_counter()`.
*   **Calculations:**
    *   **Serial CPU Time:** Timed execution of the pure Python fallback.
    *   **Parallel GPU Time:** Timed execution encompassing PyCUDA memory loads and kernel runtime.
    *   **Speedup ($S$):** $T_{serial} / T_{parallel}$. The result factor is returned to the UI chart.

## 4. Run/Execution Environment
*   **Testing / Cloud Platform:** Google Colab (Providing Free NVIDIA T4 GPUs)
*   **Run Commands:**
    *   *Backend:* `uvicorn backend.main:app --host 0.0.0.0 --port 8000`
    *   *Frontend:* Standard local HTTP server or Ngrok forwarding for the Colab notebook.
