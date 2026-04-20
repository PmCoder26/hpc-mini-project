import numpy as np

try:
    import pycuda.driver as cuda
    import pycuda.autoinit
    from pycuda.compiler import SourceModule
    CUDA_AVAILABLE = True
except ImportError:
    CUDA_AVAILABLE = False
except Exception:
    CUDA_AVAILABLE = False

if CUDA_AVAILABLE:
    # Defining CUDA Kernels using atomicAdd for compaction filter
    kernel_code = """
    extern "C" {
        // Numeric Filter Kernel (Greater Than) - Float Arrays
        __global__ void filter_gt_float(float *data, float threshold, int *out_indices, int *count, int n) {
            int idx = blockIdx.x * blockDim.x + threadIdx.x;
            if(idx < n) {
                if (data[idx] > threshold) {
                    int pos = atomicAdd(count, 1);
                    out_indices[pos] = idx;
                }
            }
        }
        
        // Match Filter Kernel (Equals) - Int Arrays
        __global__ void filter_eq_int(int *data, int target, int *out_indices, int *count, int n) {
            int idx = blockIdx.x * blockDim.x + threadIdx.x;
            if(idx < n) {
                if (data[idx] == target) {
                    int pos = atomicAdd(count, 1);
                    out_indices[pos] = idx;
                }
            }
        }
        
        // Sum Reduction Kernel (Partial sum block-wise)
        __global__ void reduce_sum_float(float *input, float *output, int n) {
            // Shared memory size must be provided during launch
            extern __shared__ float sdata[];
            int tid = threadIdx.x;
            int i = blockIdx.x * blockDim.x + threadIdx.x;
            
            sdata[tid] = (i < n) ? input[i] : 0;
            __syncthreads();
            
            for(int s = blockDim.x / 2; s > 0; s >>= 1) {
                if (tid < s) {
                    sdata[tid] += sdata[tid + s];
                }
                __syncthreads();
            }
            
            if (tid == 0) output[blockIdx.x] = sdata[0];
        }
    }
    """
    mod = SourceModule(kernel_code, no_extern_c=True)
    filter_gt_float = mod.get_function("filter_gt_float")
    filter_eq_int = mod.get_function("filter_eq_int")
    reduce_sum_float = mod.get_function("reduce_sum_float")

class ParallelAlgorithms:
    
    @staticmethod
    def _execute_filter_kernel(kernel, data_arr, threshold, n):
        out_indices_gpu = cuda.mem_alloc(data_arr.nbytes)
        count_cpu = np.zeros(1, dtype=np.int32)
        count_gpu = cuda.mem_alloc(count_cpu.nbytes)
        data_gpu = cuda.mem_alloc(data_arr.nbytes)
        
        cuda.memcpy_htod(data_gpu, data_arr)
        cuda.memcpy_htod(count_gpu, count_cpu)
        
        block_size = 256
        grid_size = int(np.ceil(n / block_size))
        
        kernel(data_gpu, threshold, out_indices_gpu, count_gpu, np.int32(n), block=(block_size, 1, 1), grid=(grid_size, 1))
        
        cuda.memcpy_dtoh(count_cpu, count_gpu)
        total_matches = count_cpu[0]
        
        if total_matches > 0:
            final_gpu_indices = np.empty(total_matches, dtype=np.int32)
            cuda.memcpy_dtoh(final_gpu_indices, out_indices_gpu)
            return final_gpu_indices
        return np.array([], dtype=np.int32)

    @staticmethod
    def filterEmployees(employees: dict, threshold_salary: float):
        if not CUDA_AVAILABLE: return SerialAlgorithms.filterEmployees(employees, threshold_salary)
        data_arr = employees['salary']
        n = len(data_arr)
        threshold = np.float32(threshold_salary)
        return ParallelAlgorithms._execute_filter_kernel(filter_gt_float, data_arr, threshold, n)

    @staticmethod
    def filterDepartments(departments: dict, target_dept_id: int):
        if not CUDA_AVAILABLE: return SerialAlgorithms.filterDepartments(departments, target_dept_id)
        data_arr = departments['dept_id']
        n = len(data_arr)
        threshold = np.int32(target_dept_id)
        return ParallelAlgorithms._execute_filter_kernel(filter_eq_int, data_arr, threshold, n)

    @staticmethod
    def filterPayroll(payroll: dict, min_net_paid: float):
        if not CUDA_AVAILABLE: return SerialAlgorithms.filterPayroll(payroll, min_net_paid)
        data_arr = payroll['net_paid']
        n = len(data_arr)
        threshold = np.float32(min_net_paid)
        return ParallelAlgorithms._execute_filter_kernel(filter_gt_float, data_arr, threshold, n)

    @staticmethod
    def filterAttendance(attendance: dict, target_status: int):
        if not CUDA_AVAILABLE: return SerialAlgorithms.filterAttendance(attendance, target_status)
        data_arr = attendance['status']
        n = len(data_arr)
        threshold = np.int32(target_status)
        return ParallelAlgorithms._execute_filter_kernel(filter_eq_int, data_arr, threshold, n)

