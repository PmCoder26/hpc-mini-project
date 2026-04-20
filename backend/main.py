import os
from fastapi import FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel

from backend.database_engine import DatabaseEngine
from backend.query_optimizer import QueryOptimizer

app = FastAPI(title="HPC Analytics Backend (OOP Architecture)")

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_methods=["*"],
    allow_headers=["*"],
)

# Instantiate Architecture Classes
engine = DatabaseEngine()
optimizer = QueryOptimizer(engine)

@app.on_event("startup")
async def startup_event():
    data_dir = os.path.join(os.path.dirname(os.path.dirname(__file__)), 'data')
    engine.initialize(data_dir)

class FilterRequest(BaseModel):
    field: str      
    value: float
    limit: int = 100

@app.get("/")
def read_root():
    from backend.parallel_algorithms import CUDA_AVAILABLE
    return {"status": "HPC Backend Running - OOP Architecture Active", "cuda_available": CUDA_AVAILABLE}

@app.post("/api/filter/employees")
def api_filter_employees(req: FilterRequest):
    if req.field != 'salary':
        raise HTTPException(status_code=400, detail="Only 'salary' filter is supported in this architecture node")
    return optimizer.runEmployeeFilter(req.value, req.limit)

@app.post("/api/filter/departments")
def api_filter_departments(req: FilterRequest):
    if req.field != 'dept_id':
        raise HTTPException(status_code=400, detail="Only 'dept_id' filter is supported")
    return optimizer.runDepartmentFilter(int(req.value), req.limit)

@app.post("/api/filter/payroll")
def api_filter_payroll(req: FilterRequest):
    if req.field != 'net_paid':
        raise HTTPException(status_code=400, detail="Only 'net_paid' filter is supported")
    return optimizer.runPayrollFilter(req.value, req.limit)

@app.post("/api/filter/attendance")
def api_filter_attendance(req: FilterRequest):
    if req.field != 'status':
        raise HTTPException(status_code=400, detail="Only 'status' filter is supported")
    return optimizer.runAttendanceFilter(int(req.value), req.limit)
