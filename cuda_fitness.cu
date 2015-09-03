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

__device__ void CCompFit(int bid, int tid, int bdim, int* s_total_sta, int* s_min_exe,
			 float* c_time_vec, int* c_gene_map, float* c_fitness_vec, int test_num, int code_num) {
  for (int i = (bdim / 2); i > 0; i /= 2) {
    if (tid < i && tid + i < code_num) {
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

  for (int i = 0; i <= min_num; i += bdim) {
    if (tid + i <= min_num) {
      int gene_code = c_gene_map[bid * test_num + tid + i];
      s_exe_time_vec[tid] += c_time_vec[gene_code];
    }
  }

  //  for (int i = (bdim / 2); i > 0 && i <= min_num ; i /= 2) {
  for (int i = (bdim / 2); i > 0 ; i /= 2) {
    if (tid < i) {
      s_exe_time_vec[tid] += s_exe_time_vec[tid + i];
    }
  }

  if (tid == 0) {
    c_fitness_vec[bid * 2] = (1.0f - s_total_sta[0] * 1.0f / (code_num * test_num) + 1.0f / (test_num << 1));
    //    c_fitness_vec[bid * 2] = s_total_sta[0];
    c_fitness_vec[bid * 2 + 1] = s_exe_time_vec[0];
    //    c_fitness_vec[bid * 2 + 1] = min_num;
    //    c_fitness_vec[bid * 2 + 1] = c_gene_map[bid * test_num + 50];
    //    c_fitness_vec[bid * 2 + 1] = min_num;
    //    c_fitness_vec[bid * 2 + 1] = 0;
  }

  __syncthreads();
}

__global__ void CInitFit(char* c_cover_map, float* c_time_vec, int* c_gene_map, 
                         int* c_first_exe_map, float* c_fitness_vec, int test_num, int code_num) {
  int bid = blockIdx.x;
  int tid = threadIdx.x;
  int bdim = MAXTHREAD;

  __shared__ int s_min_exe[MAXTHREAD];
  s_min_exe[tid] = MIN_NUM;
  __syncthreads();

  __shared__ int s_total_sta[MAXTHREAD];
  s_total_sta[tid] = 0;
  __syncthreads();

  int gene_code;
  int first;
  for(int i = 0; i < code_num; i += bdim) {
    if((tid + i) < code_num) {
      first = test_num;
      for (int j = 0; j < test_num; ++j) {
	gene_code = c_gene_map[bid * test_num + j];
	if (c_cover_map[gene_code * code_num + tid + i] == '1') {
	  first = j;
	  break;
	}
      }
      if (first == test_num) {
	// Uncoverd.
	c_first_exe_map[bid * code_num + tid + i] = NOEXIST;
      } else {
	c_first_exe_map[bid * code_num + tid + i] = first;
	s_total_sta[tid] += (first + 1);
	s_min_exe[tid] = max(s_min_exe[tid], first);
      }
      __syncthreads();
    }
  }
  __syncthreads();

  CCompFit(bid, tid, bdim, s_total_sta, s_min_exe,
    	   c_time_vec, c_gene_map, c_fitness_vec, test_num, code_num);

  __syncthreads();
}

void CudaCompFit(CudaTimeRecord& time_rec, int com_num) {
  for (int i = 0; i < com_num; ++i) {
    for (int j = 0; j < test_num; ++j) {
      t_gene_map[i * test_num + j] = pop_vec[i].gene_code_vec[j];
    }
  }

  for (int i = 0; i < com_num; ++i) {
    for (int j = 0; j < code_num; ++j) {
      t_first_exe_map[i * code_num + j] = pop_vec[i].first_exe_vec[j];
    }
  }

  time_rec.Start();

  cudaGetErrorString(cudaMemcpy(c_gene_map, t_gene_map, pop_num * test_num * 2 * sizeof(int), 
                                cudaMemcpyHostToDevice));

  cout << "initfit kernel start" << endl;
  CInitFit<<<com_num, MAXTHREAD>>>(c_cover_map, c_time_vec, c_gene_map, c_first_exe_map, c_fitness_vec, test_num, code_num);

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

void CudaCompFit2(CudaTimeRecord& time_rec, CudaTimeRecord& time_in_rec, CudaTimeRecord& time_out_rec, int com_num) {
  for (int i = 0; i < pop_num; ++i) {
    for (int j = 0; j < test_num; ++j) {
      t_gene_map[i * test_num + j] = pop_vec[pop_num + i].gene_code_vec[j];
    }
  }


  time_in_rec.Start();
  cudaGetErrorString(cudaMemcpy(c_gene_map, t_gene_map, pop_num * test_num * sizeof(int), 
                                cudaMemcpyHostToDevice));
  time_in_rec.Record();

  cout << "initfit kernel start" << endl;
  time_rec.Start();
  CInitFit<<<pop_num, MAXTHREAD>>>(c_cover_map, c_time_vec, c_gene_map, c_first_exe_map, c_fitness_vec, test_num, code_num);
  time_rec.Record();

  cout << "muta kernel end" << endl;
  time_out_rec.Start();
  cudaGetErrorString(cudaMemcpy(t_fitness_vec, c_fitness_vec, pop_num * 4 * sizeof(float),
                                cudaMemcpyDeviceToHost));
  time_out_rec.Record();


  for (int i = 0; i < pop_num; ++i) {
    pop_vec[pop_num + i].apsc = t_fitness_vec[i * 2];
    pop_vec[pop_num + i].eff_time = t_fitness_vec[i * 2 + 1];
  }
}
