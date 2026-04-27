import os
import sys
from fastapi import FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel

# =========================================================
# ✅ COLAB PATH FIX
# =========================================================
BASE_DIR = "/content/hpc-mini-project"
sys.path.append(BASE_DIR)

from backend.database_engine import DatabaseEngine
from backend.query_optimizer import QueryOptimizer

app = FastAPI(title="HPC Analytics Backend (Colab)")

# ---------------- CORS ----------------
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_methods=["*"],
    allow_headers=["*"],
)

# ---------------- INIT ----------------
engine = DatabaseEngine()
optimizer = QueryOptimizer(engine)

# ---------------- STARTUP ----------------
@app.on_event("startup")
async def startup_event():
    data_dir = "/content/hpc-mini-project/data"   # ✅ HARD FIX FOR COLAB

    print("[STARTUP] Loading dataset from:", data_dir)
    engine.initialize(data_dir)

# ---------------- MODEL ----------------
class FilterRequest(BaseModel):
    field: str
    value: float
    limit: int = 100

# ---------------- ROOT ----------------
@app.get("/")
def root():
    from backend.parallel_algorithms import CUDA_AVAILABLE

    return {
        "status": "HPC Backend Running (Colab + GPU T4)",
        "cuda_available": CUDA_AVAILABLE
    }

# ================= EMPLOYEES =================
@app.post("/api/filter/employees")
def employees(req: FilterRequest):
    if req.field != "salary":
        raise HTTPException(400, "Only salary filter allowed")

    return optimizer.runEmployeeFilter(req.value, req.limit)

# ================= DEPARTMENTS =================
@app.post("/api/filter/departments")
def departments(req: FilterRequest):
    if req.field != "dept_id":
        raise HTTPException(400, "Only dept_id filter allowed")

    return optimizer.runDepartmentFilter(int(req.value), req.limit)

# ================= PAYROLL =================
@app.post("/api/filter/payroll")
def payroll(req: FilterRequest):
    if req.field != "net_paid":
        raise HTTPException(400, "Only net_paid filter allowed")

    return optimizer.runPayrollFilter(req.value, req.limit)

# ================= ATTENDANCE =================
@app.post("/api/filter/attendance")
def attendance(req: FilterRequest):
    if req.field != "status":
        raise HTTPException(400, "Only status filter allowed")

    return optimizer.runAttendanceFilter(int(req.value), req.limit)