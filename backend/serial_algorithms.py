import numpy as np

class SerialAlgorithms:
    @staticmethod
    def filterEmployees(employees: dict, threshold_salary: float):
        """Returns indices of employees with salary > threshold"""
        return np.where(employees['salary'] > threshold_salary)[0]

    @staticmethod
    def filterDepartments(departments: dict, target_dept_id: int):
        """Returns indices of department matching ID"""
        return np.where(departments['dept_id'] == target_dept_id)[0]

    @staticmethod
    def filterPayroll(payroll: dict, min_net_paid: float):
        """Returns indices of payroll records greater than amount"""
        return np.where(payroll['net_paid'] > min_net_paid)[0]

    @staticmethod
    def filterAttendance(attendance: dict, target_status: int):
        """Returns indices matching a specific attendance status (e.g. 0=Absent)"""
        return np.where(attendance['status'] == target_status)[0]

    @staticmethod
    def aggregatePayroll(payroll: dict) -> float:
        """Returns sum of all net paid"""
        return float(np.sum(payroll['net_paid']))
