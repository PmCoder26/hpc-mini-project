import numpy as np

# ================= SAFE CUDA CHECK =================
try:
    import pycuda.driver as cuda
    import pycuda.autoinit
    from pycuda.compiler import SourceModule

    CUDA_AVAILABLE = True
except Exception as e:
    print("[CUDA Disabled]", e)
    CUDA_AVAILABLE = False


# ================= FALLBACK SERIAL =================
class SerialFallback:
    @staticmethod
    def filter_gt(arr, threshold):
        return np.where(arr > threshold)[0]

    @staticmethod
    def filter_eq(arr, target):
        return np.where(arr == target)[0]


# ================= CUDA KERNELS =================
if CUDA_AVAILABLE:
    try:
        kernel_code = r"""
        extern "C" {

        __global__ void filter_gt_float(float *data, float threshold, int *out, int *count, int n) {
            int i = blockIdx.x * blockDim.x + threadIdx.x;
            if (i < n && data[i] > threshold) {
                int pos = atomicAdd(count, 1);
                out[pos] = i;
            }
        }

        __global__ void filter_eq_int(int *data, int target, int *out, int *count, int n) {
            int i = blockIdx.x * blockDim.x + threadIdx.x;
            if (i < n && data[i] == target) {
                int pos = atomicAdd(count, 1);
                out[pos] = i;
            }
        }

        }
        """

        mod = SourceModule(kernel_code)
        filter_gt_float = mod.get_function("filter_gt_float")
        filter_eq_int = mod.get_function("filter_eq_int")

    except Exception as e:
        print("[CUDA Kernel Compilation Failed]", e)
        CUDA_AVAILABLE = False


# ================= MAIN CLASS =================
class ParallelAlgorithms:

    @staticmethod
    def _execute(kernel, data, threshold, n, dtype="float"):
        if not CUDA_AVAILABLE:
            return None

        try:
            data_gpu = cuda.mem_alloc(data.nbytes)
            out_gpu = cuda.mem_alloc(data.nbytes)
            count_gpu = cuda.mem_alloc(np.int32().nbytes)

            count = np.zeros(1, dtype=np.int32)

            cuda.memcpy_htod(data_gpu, data)
            cuda.memcpy_htod(count_gpu, count)

            block = 256
            grid = (n + block - 1) // block

            kernel(
                data_gpu,
                np.float32(threshold) if dtype == "float" else np.int32(threshold),
                out_gpu,
                count_gpu,
                np.int32(n),
                block=(block, 1, 1),
                grid=(grid, 1),
            )

            cuda.memcpy_dtoh(count, count_gpu)
            result = np.empty(count[0], dtype=np.int32)
            cuda.memcpy_dtoh(result, out_gpu)

            return result

        except Exception as e:
            print("[CUDA Runtime Error]", e)
            return None

    # =============== EMPLOYEES ===============
    @staticmethod
    def filterEmployees(employees, threshold):
        arr = employees["salary"]
        n = len(arr)

        if not CUDA_AVAILABLE:
            return SerialFallback.filter_gt(arr, threshold)

        result = ParallelAlgorithms._execute(
            filter_gt_float, arr, threshold, n, "float"
        )

        return result if result is not None else SerialFallback.filter_gt(arr, threshold)

    # =============== DEPARTMENTS ===============
    @staticmethod
    def filterDepartments(departments, target):
        arr = departments["dept_id"]
        n = len(arr)

        if not CUDA_AVAILABLE:
            return SerialFallback.filter_eq(arr, target)

        result = ParallelAlgorithms._execute(
            filter_eq_int, arr, target, n, "int"
        )

        return result if result is not None else SerialFallback.filter_eq(arr, target)

    # =============== PAYROLL ===============
    @staticmethod
    def filterPayroll(payroll, threshold):
        arr = payroll["net_paid"]
        n = len(arr)

        if not CUDA_AVAILABLE:
            return SerialFallback.filter_gt(arr, threshold)

        result = ParallelAlgorithms._execute(
            filter_gt_float, arr, threshold, n, "float"
        )

        return result if result is not None else SerialFallback.filter_gt(arr, threshold)

    # =============== ATTENDANCE ===============
    @staticmethod
    def filterAttendance(attendance, target):
        arr = attendance["status"]
        n = len(arr)

        if not CUDA_AVAILABLE:
            return SerialFallback.filter_eq(arr, target)

        result = ParallelAlgorithms._execute(
            filter_eq_int, arr, target, n, "int"
        )

        return result if result is not None else SerialFallback.filter_eq(arr, target)