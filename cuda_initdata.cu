#include <iostream>
#include "cuda_initdata.h"
#include "initdata.h"
#include "gene.h"
#include "time_record.h"

#include <cstdlib>
#include <cuda_runtime.h>

float* c_time_vec = NULL;
int* c_gene_map = NULL;
int* c_first_exe_map = NULL;
int *c_rand_vec = NULL;
int *c_father_vec = NULL;
float *c_fitness_vec = NULL;
int* c_single_pos_map = NULL;
int* c_swap_vec = NULL;
char* c_cover_map = NULL;
int* c_is_cover_vec = NULL;
int* c_pos_map = NULL;

// Temp var.
float * t_time_vec = NULL;
int* t_gene_map = NULL;
int* t_first_exe_map = NULL;
int *t_rand_vec = NULL;
int *t_father_vec = NULL;
float* t_fitness_vec = NULL;
int* t_single_pos_map = NULL;
int* t_swap_vec = NULL;
char* t_cover_map = NULL;
int* t_is_cover_vec = NULL;
int* t_pos_map = NULL;

// @description: initialize the data nsga2 need
// @return
// 0 success
// -1 failed to read data
// int CudaInitData(TimeRecord& c);
int CudaInitData(TimeRecord& c) {
  cout << "int CudaInitData(TimeRecord& c) {" << endl;
  getchar();
  if (InitData() != 0) {
    return -1;
  }
  srand(time(NULL));
  // Temp var.
  t_time_vec = new float[test_num];
  t_gene_map = new int[pop_num * test_num * 2];
  t_first_exe_map = new int[pop_num * code_num * 2];
  t_rand_vec = new int[pop_num];
  t_father_vec = new int[pop_num];
  t_fitness_vec = new float[pop_num * 4];
  t_single_pos_map = new int[pop_num * test_num];
  t_swap_vec = new int[pop_num * 4];
  t_cover_map = new char[test_num * code_num];
  t_is_cover_vec = new int[code_num];
  t_pos_map = new int[pop_num * test_num];

  for (int i = 0; i < test_num; ++i) {
    t_time_vec[i] = gen_vec[i].time;
  }

  for (int i = 0; i < test_num; ++i) {
    for (int j = 0; j < code_num; ++j) {
      t_cover_map[i * code_num + j] = gen_vec[i].coverage[j];
    }
  }

  for (int i = 0; i < code_num; ++i) {
    t_is_cover_vec[i] = 0;
  }
  for (int i = 0; i < code_num; ++i) {
    for (int j = 0; j < test_num; ++j) {
      if (gen_vec[j].coverage[i] == '1') {
        t_is_cover_vec[i] = 1;
        break;
      }
    }
  }

  c.Start();
  CUDA_CALL(cudaMalloc((void**)&c_time_vec, test_num * sizeof(float)));
  CUDA_CALL(cudaMalloc((void**)&c_gene_map, pop_num * test_num * 2 * sizeof(int)));
  CUDA_CALL(cudaMalloc((void**)&c_first_exe_map, pop_num * code_num * 2 * sizeof(int)));
  CUDA_CALL(cudaMalloc((void**)&c_rand_vec, pop_num * sizeof(int)));
  CUDA_CALL(cudaMalloc((void**)&c_father_vec, pop_num * sizeof(int)));
  CUDA_CALL(cudaMalloc((void**)&c_fitness_vec, (pop_num * 4) * sizeof(float)));
  CUDA_CALL(cudaMalloc((void**)&c_single_pos_map, pop_num * test_num * sizeof(int)));
  CUDA_CALL(cudaMalloc((void**)&c_swap_vec, pop_num * 4 * sizeof(int)));
  CUDA_CALL(cudaMalloc((void**)&c_cover_map, test_num * code_num * sizeof(char)));
  CUDA_CALL(cudaMalloc((void**)&c_is_cover_vec, code_num * sizeof(int)));
  CUDA_CALL(cudaMalloc((void**)&c_pos_map, pop_num * test_num * sizeof(int)));

  cudaGetErrorString(cudaMemcpy(c_time_vec, t_time_vec, test_num * sizeof(float),
                                cudaMemcpyHostToDevice));
  cudaGetErrorString(cudaMemcpy(c_cover_map, t_cover_map, test_num * code_num * sizeof(char),
                                cudaMemcpyHostToDevice));
  cudaGetErrorString(cudaMemcpy(c_is_cover_vec, t_is_cover_vec, code_num * sizeof(int),
                                cudaMemcpyHostToDevice));
  c.Record();

  return 0;
}

void CudaDeleteData() {
  t_time_vec = new float[test_num];
  t_gene_map = new int[pop_num * test_num * 2];
  t_first_exe_map = new int[pop_num * code_num * 2];
  t_rand_vec = new int[pop_num];
  t_father_vec = new int[pop_num];
  t_fitness_vec = new float[pop_num * 4];
  t_single_pos_map = new int[pop_num * test_num];
  t_swap_vec = new int[pop_num * 4];
  t_cover_map = new char[test_num * code_num];
  t_is_cover_vec = new int[code_num];
  t_pos_map = new int[pop_num * test_num];

  delete []t_time_vec;
  delete []t_gene_map;
  delete []t_first_exe_map;
  delete []t_rand_vec;
  delete []t_father_vec;
  delete []t_fitness_vec;
  delete []t_single_pos_map;
  delete []t_swap_vec;
  delete []t_cover_map;
  delete []t_is_cover_vec;
  delete []t_pos_map;

  cudaFree(c_time_vec);
  cudaFree(c_gene_map);
  cudaFree(c_first_exe_map);
  cudaFree(c_rand_vec);
  cudaFree(c_father_vec);
  cudaFree(c_fitness_vec);
  cudaFree(c_swap_vec);
  cudaFree(c_cover_map);
  cudaFree(c_pos_map);
  cudaFree(c_single_pos_map);
  cudaFree(c_is_cover_vec);
}
