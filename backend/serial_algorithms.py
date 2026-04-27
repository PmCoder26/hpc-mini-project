import numpy as np


class SerialAlgorithms:

    # =====================================================
    # EMPLOYEES
    # =====================================================
    @staticmethod
    def filterEmployees(employees: dict, threshold_salary: float):
        """
        Returns indices of employees with salary > threshold
        """
        try:
            return np.where(
                employees["salary"] > threshold_salary
            )[0]
        except Exception as e:
            print("[Serial ERROR - Employees]", e)
            return np.array([], dtype=np.int32)

    # =====================================================
    # DEPARTMENTS
    # =====================================================
    @staticmethod
    def filterDepartments(departments: dict, target_dept_id: int):
        """
        Returns indices where dept_id == target_dept_id
        """
        try:
            return np.where(
                departments["dept_id"] == target_dept_id
            )[0]
        except Exception as e:
            print("[Serial ERROR - Departments]", e)
            return np.array([], dtype=np.int32)

    # =====================================================
    # PAYROLL
    # =====================================================
    @staticmethod
    def filterPayroll(payroll: dict, min_net_paid: float):
        """
        Returns indices where net_paid > threshold
        """
        try:
            return np.where(
                payroll["net_paid"] > min_net_paid
            )[0]
        except Exception as e:
            print("[Serial ERROR - Payroll]", e)
            return np.array([], dtype=np.int32)

    # =====================================================
    # ATTENDANCE
    # =====================================================
    @staticmethod
    def filterAttendance(attendance: dict, target_status: int):
        """
        status mapping:
        1 = Present
        0 = Absent
        2 = Leave
        """
        try:
            return np.where(
                attendance["status"] == target_status
            )[0]
        except Exception as e:
            print("[Serial ERROR - Attendance]", e)
            return np.array([], dtype=np.int32)

    # =====================================================
    # OPTIONAL AGGREGATION (SAFE)
    # =====================================================
    @staticmethod
    def aggregatePayroll(payroll: dict) -> float:
        """
        Sum of all net paid values
        """
        try:
            return float(np.sum(payroll["net_paid"]))
        except Exception as e:
            print("[Serial ERROR - Aggregate Payroll]", e)
            return 0.0