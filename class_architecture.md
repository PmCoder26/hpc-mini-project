# Web API and Module Architecture

This document maps out the Python file structure, API endpoints, Data representation (Pydantic Models), and Core classes for the **PyCUDA Analytics Engine**.

## 1. Pydantic Models (Entities)
We use `pydantic.BaseModel` to strictly type-enforce the JSON payload coming in from the Web Dashboard.

```python
# Defined in main.py
class FilterRequest(BaseModel):
    field: str        # e.g., "salary", "net_paid", "dept_id"
    value: float      # e.g., 50000, 3
    limit: int = 100  # Limits the number of records returned to the web UI
```

---

## 2. Core Python Classes

The system logic is divided into highly decoupled Python files. The central engine holds state (dataframes), while algorithm classes provide the exact static methods needed by the optimizer.

### `DatabaseEngine` (`backend/database_engine.py`)
The central state hub that holds the loaded arrays in RAM. It abstracts Pandas File I/O.
```python
class DatabaseEngine:
    def __init__(self):
        # Stored as pandas.DataFrame natively
        self.employees = None
        self.departments = None
        self.payroll = None
        self.attendance = None

    def initialize(self, directory="data"):
        # Uses pd.read_csv to load 4 generated datasets...
        pass
        
    # Getters returning DataFrames or exact series
    def getEmployees(self): ...
    def getDepartments(self): ...
```

### `SerialAlgorithms` (`backend/serial_algorithms.py`)
The baseline, pure Python implementations of database operations. These contain manual `for` loops across properties.
```python
class SerialFallback:
    @staticmethod
    def filter_gt(arr, threshold):
        # Loops across len(arr), appends matched indices.
        pass

    @staticmethod
    def filter_eq(arr, target):
        # Loops across len(arr), checking for exact target match.
        pass
```

### `ParallelAlgorithms` (`backend/parallel_algorithms.py`)
The HPC Core. This file intercepts PyCUDA drivers, executes C++ kernels as `SourceModule` and handles VRAM memory allocation.
```python
class ParallelAlgorithms:
    @staticmethod
    def _execute_compaction(kernel, arr, value, n, dtype="float"):
        # 1. cuda.mem_alloc() array and counter memory
        # 2. cuda.memcpy_htod()
        # 3. Executes `kernel()`
        # 4. cuda.memcpy_dtoh() result logic
        pass

    @staticmethod
    def filterEmployees(employees, threshold):
        # Executes filter_gt_compact kernel across CUDA device
        pass

    @staticmethod
    def filterDepartments(departments, target):
        # Executes filter_eq_compact kernel across CUDA device
        pass
```

### `QueryOptimizer` (`backend/query_optimizer.py`)
The analytical orchestrator. It receives data from the `DatabaseEngine`, runs BOTH serial and parallel methods sequentially, clocks their time with `time.perf_counter()`, and constructs the final JSON payload.
```python
class QueryOptimizer:
    def __init__(self, engine):
        self.engine = engine

    # Shared orchestration wrapper
    def _execute_and_compare(self, name, data, target, parallel_func, serial_func, limit=100):
        # Generates metrics:
        # {
        #   "cpu_time_sec": ..., 
        #   "gpu_time_sec": ..., 
        #   "speedup_factor": ...,
        #   "matches_found": ...
        # }
        pass

    def runEmployeeFilter(self, salary_min, limit): ...
    def runPayrollFilter(self, net_paid_min, limit): ...
```

---

## 3. Web Service Component (`backend/main.py`)
The application entry point is built on FastAPI. It configures CORS (allowing the frontend to communicate), instantiates the global `DatabaseEngine` upon server start, and maps routes.

```python
# FastAPI Endpoints
@app.on_event("startup")
async def startup_event(): ... # Triggers DatabaseEngine.initialize()

@app.get("/")
def root(): ... # Sanity Check & returning CUDA_AVAILABLE bool

@app.post("/api/filter/employees")
def employees(req: FilterRequest): ...

@app.post("/api/filter/departments")
def departments(req: FilterRequest): ...

@app.post("/api/filter/payroll")
def payroll(req: FilterRequest): ...

@app.post("/api/filter/attendance")
def attendance(req: FilterRequest): ...
```
