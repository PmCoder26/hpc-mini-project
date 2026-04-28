import numpy as np

# ================= CUDA CHECK =================
try:
    import pycuda.driver as cuda
    import pycuda.autoinit
    from pycuda.compiler import SourceModule

    CUDA_AVAILABLE = True
except Exception as e:
    print("[CUDA Disabled]", e)
    CUDA_AVAILABLE = False


# ================= SERIAL FALLBACK (PURE PYTHON) =================
class SerialFallback:

    @staticmethod
    def filter_gt(arr, threshold):
        result = []
        for i in range(len(arr)):
            if arr[i] > threshold:
                result.append(i)
        return result

    @staticmethod
    def filter_eq(arr, target):
        result = []
        for i in range(len(arr)):
            if arr[i] == target:
                result.append(i)
        return result


# ================= CUDA KERNEL =================
if CUDA_AVAILABLE:
    kernel_code = r"""
    extern "C" {

    __global__ void filter_gt_compact(float *data, float threshold, int *out, int *count, int n) {
        int i = blockIdx.x * blockDim.x + threadIdx.x;

        if (i < n) {
            if (data[i] > threshold) {
                int pos = atomicAdd(count, 1);
                out[pos] = i;
            }
        }
    }

    __global__ void filter_eq_compact(int *data, int target, int *out, int *count, int n) {
        int i = blockIdx.x * blockDim.x + threadIdx.x;

        if (i < n) {
            if (data[i] == target) {
                int pos = atomicAdd(count, 1);
                out[pos] = i;
            }
        }
    }

    }
    """

    mod = SourceModule(kernel_code)
    filter_gt_compact = mod.get_function("filter_gt_compact")
    filter_eq_compact = mod.get_function("filter_eq_compact")


# ================= MAIN CLASS =================
class ParallelAlgorithms:

    @staticmethod
    def _execute_compaction(kernel, arr, value, n, dtype="float"):
        if not CUDA_AVAILABLE:
            return None

        try:
            # Allocate GPU memory
            data_gpu = cuda.mem_alloc(arr.nbytes)
            out_gpu = cuda.mem_alloc(n * np.int32().nbytes)
            count_gpu = cuda.mem_alloc(np.int32().nbytes)

            # Initialize counter
            cuda.memcpy_htod(count_gpu, np.array([0], dtype=np.int32))

            # Copy input
            cuda.memcpy_htod(data_gpu, arr)

            block = 256
            grid = (n + block - 1) // block

            # Launch kernel
            kernel(
                data_gpu,
                np.float32(value) if dtype == "float" else np.int32(value),
                out_gpu,
                count_gpu,
                np.int32(n),
                block=(block, 1, 1),
                grid=(grid, 1),
            )

            # Get result count
            count = np.zeros(1, dtype=np.int32)
            cuda.memcpy_dtoh(count, count_gpu)
            result_size = int(count[0])

            # Copy only valid portion
            result = np.empty(result_size, dtype=np.int32)
            cuda.memcpy_dtoh(result, out_gpu)

            return result.tolist()

        except Exception as e:
            print("[CUDA ERROR]", e)
            return None


    # ================= EMPLOYEES =================
    @staticmethod
    def filterEmployees(employees, threshold):
        arr = employees["salary"]
        n = len(arr)

        if not CUDA_AVAILABLE:
            return SerialFallback.filter_gt(arr, threshold)

        result = ParallelAlgorithms._execute_compaction(
            filter_gt_compact, arr, threshold, n, "float"
        )

        return result if result is not None else SerialFallback.filter_gt(arr, threshold)


    # ================= DEPARTMENTS =================
    @staticmethod
    def filterDepartments(departments, target):
        arr = departments["dept_id"]
        n = len(arr)

        if not CUDA_AVAILABLE:
            return SerialFallback.filter_eq(arr, target)

        result = ParallelAlgorithms._execute_compaction(
            filter_eq_compact, arr, target, n, "int"
        )

        return result if result is not None else SerialFallback.filter_eq(arr, target)


    # ================= PAYROLL =================
    @staticmethod
    def filterPayroll(payroll, threshold):
        arr = payroll["net_paid"]
        n = len(arr)

        if not CUDA_AVAILABLE:
            return SerialFallback.filter_gt(arr, threshold)

        result = ParallelAlgorithms._execute_compaction(
            filter_gt_compact, arr, threshold, n, "float"
        )

        return result if result is not None else SerialFallback.filter_gt(arr, threshold)


    # ================= ATTENDANCE =================
    @staticmethod
    def filterAttendance(attendance, target):
        arr = attendance["status"]
        n = len(arr)

        if not CUDA_AVAILABLE:
            return SerialFallback.filter_eq(arr, target)

        result = ParallelAlgorithms._execute_compaction(
            filter_eq_compact, arr, target, n, "int"
        )

        return result if result is not None else SerialFallback.filter_eq(arr, target)