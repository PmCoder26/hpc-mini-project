import os
import pandas as pd
import numpy as np


class DataLoader:
    
    @staticmethod
    def loadEmployees(filepath: str) -> dict:
        df = pd.read_csv(filepath)

        return {
            "df": df,
            "emp_id": df["emp_id"].values.astype(np.int32),
            "age": df["age"].values.astype(np.int32),
            "dept_id": df["dept_id"].values.astype(np.int32),
            "salary": df["salary"].values.astype(np.float32),
        }

    @staticmethod
    def loadDepartments(filepath: str) -> dict:
        df = pd.read_csv(filepath)

        return {
            "df": df,
            "dept_id": df["dept_id"].values.astype(np.int32),
        }

    @staticmethod
    def loadPayrollLogs(filepath: str) -> dict:
        df = pd.read_csv(filepath)

        return {
            "df": df,
            "emp_id": df["emp_id"].values.astype(np.int32),
            # ✅ FIXED COLUMN NAME
            "net_paid": df["net_pay"].values.astype(np.float32),
        }

    @staticmethod
    def loadAttendanceLogs(filepath: str) -> dict:
        df = pd.read_csv(filepath)

        stat_map = {
            "Present": 1,
            "Absent": 0,
            "Leave": 2
        }

        df["status_int"] = df["status"].map(stat_map).fillna(-1)

        return {
            "df": df,
            "emp_id": df["emp_id"].values.astype(np.int32),
            "status": df["status_int"].values.astype(np.int32),
        }


class DatabaseEngine:

    def __init__(self):
        self.employees = None
        self.departments = None
        self.payroll_logs = None
        self.attendance_logs = None

    def initialize(self, directory_path: str):
        """
        Loads all datasets into memory
        """
        try:
            print(f"[DatabaseEngine] Loading data from: {directory_path}")

            self.employees = DataLoader.loadEmployees(
                os.path.join(directory_path, "employees.csv")
            )

            self.departments = DataLoader.loadDepartments(
                os.path.join(directory_path, "departments.csv")
            )

            self.payroll_logs = DataLoader.loadPayrollLogs(
                os.path.join(directory_path, "payroll.csv")
            )

            self.attendance_logs = DataLoader.loadAttendanceLogs(
                os.path.join(directory_path, "attendance.csv")
            )

            print("[DatabaseEngine] Initialization complete ✔")

        except Exception as e:
            print(f"[DatabaseEngine ERROR] {e}")
            raise e   # ✅ IMPORTANT: crash fast instead of silent failure

    # ---------------- Getters ----------------

    def getEmployees(self):
        return self.employees

    def getDepartments(self):
        return self.departments

    def getPayrollLogs(self):
        return self.payroll_logs

    def getAttendanceLogs(self):
        return self.attendance_logs