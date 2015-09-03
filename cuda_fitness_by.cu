#include "cuda_fitness.h"
#include "cuda_initdata.h"
#include "initdata.h"
#include "time_record.h"
#include "cuda_time_record.h"

#include <cstdlib>
#include <cuda.h>
#include <cuda_runtime.h>
#include <cuda_runtime_api.h>
#include <device_functions.h>
#include <device_launch_parameters.h>


__global__ void CInitialFit(float* c_time_vec, int* c_gene_map, 
			    int* c_first_exe_map, float* c_fitness_vec) {

}

__global__ void CSingleCrossUpdFit(float* c_time_vec, int* c_gene_map, 
			     int* c_first_exe_map, int* c_rand_vec, 
			     int* c_father_vec, float* c_fitness_vec, 
			     int* c_single_pos_map) {
  int bid = blockIdx.x;
  int tid = threadIdx.x;
  int bdim = MAXTHREAD;

  int f1, f2;
  if (bid % 2 == 0) {
    f1 = c_father_vec[bid];
    f2 = c_father_vec[bid + 1];
  } else {
    f1 = c_father_vec[bid];
    f2 = c_father_vec[bid - 1];
  }
  __syncthreads();

  int k = c_rand_vec[bid / 2];
  __syncthreads();

  __shared__ int s_min_exe[MAXTHREAD];
  s_min_exe[tid] = MIN_NUM;
  __syncthreads();

  __shared__ int s_total_sta[MAXTHREAD];
  s_total_sta[tid] = 0;
  __syncthreads();

  for(int i = 0; i < CODE_NUM; i += bdim) {
    if((tid + i) < CODE_NUM) {
      int t = c_first_exe_map[f1 * CODE_NUM + tid + i];
      if (t >= k) {
	int first_code = c_first_exe_map[f2 * CODE_NUM + tid + i];
        t = c_single_pos_map[bid * TEST_NUM + first_code];
      }
      c_first_exe_map[(POP_NUM + bid) * CODE_NUM + tid + i] = t;
      if (t != NOEXIST) {
        s_total_sta[tid] += (t + 1);
        s_min_exe[tid] = max(s_min_exe[tid], t);
      }
    }
    __syncthreads();
  }
  __syncthreads();
}

void CudaSinglePointCrossoverUpdFit(CudaTimeRecord& time_rec) {
  // void CudaSinglePointCrossoverUpdFit(TimeRecord& time_rec) {
  for (int i = 0; i < pop_num; ++i) {
    for (int j = 0; j < test_num; ++j) {
      t_gene_map[i * test_num + j] = pop_vec[pop_num + i].gene_code_vec[j];
    }
  }
  for (int i = 0; i < pop_num; ++i) {
    for (int j = 0; j < code_num; ++j) {
      t_first_exe_map[i * code_num + j] = pop_vec[i].first_exe_vec[j];
    }
  }

  for (int i = 0; i < pop_num; ++i) {
    t_father_vec[i] = father_sel_vec[i];
  }
  for (int i = 0; i < (pop_num >> 1); ++i) {
    t_rand_vec[i] = single_rand_vec[i];
  }
  for (int i = 0; i < pop_num; ++i) {
    for (int j = 0; j < test_num; ++j) {
      t_single_pos_map[i * test_num + j] = pop_vec[pop_num + i].single_pos_vec[j];
    }
  }

  time_rec.Start();

  cudaGetErrorString(cudaMemcpy(c_gene_map, t_gene_map, pop_num * test_num * sizeof(int), 
                                cudaMemcpyHostToDevice));
  cudaGetErrorString(cudaMemcpy(c_first_exe_map, t_first_exe_map, pop_num * code_num * sizeof(int), 
                                cudaMemcpyHostToDevice));
  cudaGetErrorString(cudaMemcpy(c_father_vec, t_father_vec, pop_num * sizeof(int), 
                                cudaMemcpyHostToDevice));
  cudaGetErrorString(cudaMemcpy(c_rand_vec, t_rand_vec, (pop_num >> 1) * sizeof(int), 
                                cudaMemcpyHostToDevice));
  cudaGetErrorString(cudaMemcpy(c_single_pos_map, t_single_pos_map, pop_num * test_num * sizeof(int), 
				cudaMemcpyHostToDevice));
  CSingleCrossUpdFit<<<pop_num, MAXTHREAD>>>
    (c_time_vec, c_gene_map, c_first_exe_map, 
     c_rand_vec, c_father_vec, c_fitness_vec, 
     c_single_pos_map);

  // Copy result out.
  cudaGetErrorString(cudaMemcpy(t_first_exe_map, c_first_exe_map + pop_num * code_num, 
				pop_num * code_num * sizeof(int), cudaMemcpyDeviceToHost));
  cudaGetErrorString(cudaMemcpy(t_fitness_vec, c_fitness_vec, (pop_num << 1) * sizeof(float),
                                cudaMemcpyDeviceToHost));

  time_rec.Record();

  for (int i = 0; i < pop_num; ++i) {
    for (int j = 0; j < code_num; ++j) {
      pop_vec[pop_num + i].first_exe_vec[j] = t_first_exe_map[i * code_num + j];
    }

    pop_vec[pop_num + i].apsc = t_fitness_vec[i * 2];
    pop_vec[pop_num + i].eff_time = t_fitness_vec[i * 2 + 1];
  }
}

//__device__ void CCompFit(int bid, int tid, int bdim, int* s_total_sta, int* s_min_exe,
//			 float* c_time_vec, int* c_gene_map, float* c_fitness_vec) {
__device__ void CCompFit(int bid, int tid, int bdim, volatile int* s_total_sta, volatile int* s_min_exe,
			 volatile float* c_time_vec, volatile int* c_gene_map, volatile float* c_fitness_vec) {

  for (int i = (bdim / 2); i > 0; i /= 2) {
    if (tid < i && tid + i < CODE_NUM) {
      s_total_sta[tid] += s_total_sta[tid + i];
      s_min_exe[tid] = max(s_min_exe[tid], s_min_exe[tid + i]);
    }
    __syncthreads();
  }
  __syncthreads();

  __shared__ float s_exe_time_vec[MAXTHREAD];
  s_exe_time_vec[tid] = 0;
  __syncthreads();

  int min_num = s_min_exe[0];
  __syncthreads();

  //  int gene_code;
  for (int i = 0; i < min_num; i += bdim) {
    /*
    gene_code = c_gene_map[bid * TEST_NUM + tid + i];
    s_exe_time_vec[tid] += c_time_vec[gene_code];
    */
    s_exe_time_vec[tid] += c_time_vec[c_gene_map[bid * TEST_NUM + tid + i]];
  }

  for (int i = (bdim / 2); i > 0; i /= 2) {
    if (tid < i && tid + i < min_num) {
      s_exe_time_vec[tid] += s_exe_time_vec[tid + i];
    }
  }
  /*
  if (tid == 0) {
    //    c_fitness_vec[bid * 2] = (1.0f - s_total_sta[0] * 1.0f / (CODE_NUM * TEST_NUM) + 1.0f / (TEST_NUM << 1));
    //    c_fitness_vec[bid * 2] = s_total_sta[0];
    c_fitness_vec[bid * 2] = -3;
    //    c_fitness_vec[bid * 2 + 1] = s_exe_time_vec[0];
    //    c_fitness_vec[bid * 2 + 1] = c_gene_map[bid * TEST_NUM + 50];
    //    c_fitness_vec[bid * 2 + 1] = min_num;
    c_fitness_vec[bid * 2 + 1] = bid;
  }
  */
  __syncthreads();
}

__global__ void CSwapMutationUpdFit(char* c_cover_map, float* c_time_vec, 
				    int* c_gene_map, int* c_first_exe_map, 
				    float* c_fitness_vec, int* c_swap_vec) {
  int bid = blockIdx.x;
  int tid = threadIdx.x;
  int bdim = MAXTHREAD;

  __shared__ int s_min_exe[MAXTHREAD];
  s_min_exe[tid] = MIN_NUM;
  __syncthreads();

  __shared__ int s_total_sta[MAXTHREAD];
  s_total_sta[tid] = 0;
  __syncthreads();

  int a = c_swap_vec[bid * 2];
  int b = c_swap_vec[bid * 2 + 1];
  __syncthreads();

  for(int i = 0; i < CODE_NUM; i += bdim) {
    if((tid + i) < CODE_NUM) {
      int first = c_first_exe_map[bid * CODE_NUM + tid + i];
      if (first != NOEXIST) {
	int gene_code;
	if (b > first) {
	  if (a == first) {
	    for (int j = a; j <= b; ++j) {
	      gene_code = c_gene_map[bid * TEST_NUM + j];
	      if (c_cover_map[gene_code * CODE_NUM + tid + i] == '1') {
		first = j;
		break;
	      }
	    }
	  } else if (a < first) {
	    gene_code = c_gene_map[bid * TEST_NUM + a];
	    if (c_cover_map[gene_code * CODE_NUM + tid + i] == '1') {
	      first = a;
	    }
	  }
	} else if (b == first) {
	  first = a;
	} else {
	  // NOTHING todo.
	}
      }
      c_first_exe_map[bid * CODE_NUM + tid + i] = first;
      s_total_sta[tid] += (first + 1);
      s_min_exe[tid] = max(s_min_exe[tid], first);
    }
    __syncthreads();
  }
  __syncthreads();

  if (tid == 0) {
    c_fitness_vec[bid * 2] = a;
    c_fitness_vec[bid * 2 + 1] = b;
  }
  __syncthreads();

  CCompFit(bid, tid, bdim, s_total_sta, s_min_exe,
  	   c_time_vec, c_gene_map, c_fitness_vec);

  __syncthreads();
}

void CudaSwapMutationUpdFit(CudaTimeRecord& time_rec) {
  //void CudaSwapMutationUpdFit(TimeRecord& time_rec) {
  cout << "start 0" << endl;
  for (int i = 0; i < pop_num * 2; ++i) {
    for (int j = 0; j < test_num; ++j) {
      t_gene_map[i * test_num + j] = pop_vec[i].gene_code_vec[j];
    }
  }

  for (int i = 0; i < pop_num * 2; ++i) {
    for (int j = 0; j < code_num; ++j) {
      t_first_exe_map[i * code_num + j] = pop_vec[i].first_exe_vec[j];
    }
  }

  for (int i = 0; i < pop_num * 2; ++i) {
    t_swap_vec[i * 2] = swap_vec_a[i];
    t_swap_vec[i * 2 + 1] = swap_vec_b[i];
  }

  cout << "time_rec" << endl;
  time_rec.Start();

  cudaGetErrorString(cudaMemcpy(c_gene_map, t_gene_map, pop_num * test_num * 2 * sizeof(int), 
                                cudaMemcpyHostToDevice));
  // TODO: May be duzzy
  cudaGetErrorString(cudaMemcpy(c_first_exe_map, t_first_exe_map, pop_num * code_num * 2 * sizeof(int), 
                                cudaMemcpyHostToDevice));
  cudaGetErrorString(cudaMemcpy(c_swap_vec, t_swap_vec, pop_num * 4 * sizeof(int), 
				cudaMemcpyHostToDevice));

  cout << "muta kernel start" << endl;
  CSwapMutationUpdFit<<<pop_num * 2, MAXTHREAD>>>(c_cover_map, c_time_vec, c_gene_map, c_first_exe_map, c_fitness_vec, c_swap_vec);

  cout << "muta kernel end" << endl;

  // Copy result out.
  cudaGetErrorString(cudaMemcpy(t_first_exe_map, c_first_exe_map,
				pop_num * code_num * 2 * sizeof(int), cudaMemcpyDeviceToHost));
  cudaGetErrorString(cudaMemcpy(t_fitness_vec, c_fitness_vec, pop_num * 4 * sizeof(float),
                                cudaMemcpyDeviceToHost));

  time_rec.Record();

  for (int i = 0; i < pop_num * 2; ++i) {
    for (int j = 0; j < code_num; ++j) {
      pop_vec[i].first_exe_vec[j] = t_first_exe_map[i * code_num + j];
    }

    pop_vec[i].apsc = t_fitness_vec[i * 2];
    pop_vec[i].eff_time = t_fitness_vec[i * 2 + 1];
  }
}
