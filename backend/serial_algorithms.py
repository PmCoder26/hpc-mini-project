import numpy as np


class SerialAlgorithms:

    # ================= PURE PYTHON FILTER =================
    @staticmethod
    def filterEmployees(employees: dict, threshold_salary: float):
        arr = employees["salary"]

        result = []
        for i in range(len(arr)):
            if arr[i] > threshold_salary:
                result.append(i)

        return np.array(result, dtype=np.int32)

    @staticmethod
    def filterDepartments(departments: dict, target_dept_id: int):
        arr = departments["dept_id"]

        result = []
        for i in range(len(arr)):
            if arr[i] == target_dept_id:
                result.append(i)

        return np.array(result, dtype=np.int32)

    @staticmethod
    def filterPayroll(payroll: dict, min_net_paid: float):
        arr = payroll["net_paid"]

        result = []
        for i in range(len(arr)):
            if arr[i] > min_net_paid:
                result.append(i)

        return np.array(result, dtype=np.int32)

    @staticmethod
    def filterAttendance(attendance: dict, target_status: int):
        arr = attendance["status"]

        result = []
        for i in range(len(arr)):
            if arr[i] == target_status:
                result.append(i)

        return np.array(result, dtype=np.int32)