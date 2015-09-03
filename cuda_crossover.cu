#include "non_dom_sort.h"
#include "initdata.h"
#include "initpop.h"
#include "gene.h"
#include "father_select.h"
#include "crossover.h"
#include "mutation.h"
#include "fitness.h"
#include "elism_select.h"
#include "time_record.h"
#include "cuda_initdata.h"
#include "cuda_fitness.h"
#include "cuda_crossover.h"
#include <iostream>
using namespace std;


// @description: Antoniol's Single Point Crossover
// generate (pop_num / 2) 

#define CHAR_BIT_NUM 8

// Two assuption:
// 1. $TEST_NUM is the power of 2;
// 2. $TEST_NUM is less or equal to 512.
__device__ void CGetSonGene(int* c_gene_map, int* c_pos_map, int p, int test_num, int pop_num) {
  int bid = blockIdx.x;
  int tid = threadIdx.x;
  int bdim = MAXTHREAD;

  for (int i = 0; i < test_num; i += bdim) {
    if (tid + i < test_num) {
      int gene_code = c_pos_map[p * test_num + tid + i];
      c_gene_map[(pop_num + bid) * test_num + tid + i] = c_gene_map[p * test_num + gene_code];
    }
  }
  __syncthreads();
}

__global__ void CCudaCross(int* c_gene_map, int* c_pos_map, int* c_father_vec, int test_num, int pop_num) {
  int bid = blockIdx.x;
  int tid = threadIdx.x;
  int bdim = MAXTHREAD;

  int p = c_father_vec[bid];
  __syncthreads();

  CGetSonGene(c_gene_map, c_pos_map, p, test_num, pop_num);

  __syncthreads();
}

void* ScanChooseCroPoint_C() {
  vector<int> flags;
  vector<int> pos;
  flags.resize(test_num);
  pos.resize(test_num);

  int t;

  // Set the random seed.
  int scan_num = test_num * cross_propo;

  for (int i = 0; i < pop_num; ++i) {
    // Initialize the vector with value 0
    flags.assign(test_num, 0);
    for (int j = 0; j < scan_num; ++j) {
      t = rand() % test_num;
      while (flags[t] == 1) {
	t = rand() % test_num;
      }
      flags[t] = 1;
    }

    // Generation the new generation using the algorithm "SCAN".
    int count = 0;
    for (int j = 0; j < test_num; ++j) {
      pos[j] = count;
      if (flags[j] == 1) {
	++count;
      }
    }

    for (int j = 0; j < test_num; ++j) {
      pop_vec[pop_num + i].flags_vec[j] = pos[j];
    }

    for (int j = 0; j < test_num; ++j) {
      if (flags[j] == 0) {
	pos[j] = scan_num + j - pos[j];
      }
    }

    for (int j = 0; j < test_num; ++j) {
      t_pos_map[test_num * i + j] = pos[j];
      //      pop_vec[pop_num + i].pos_vec[j] = pos[j];
    }
  }
}

void CudaScanCross(TimeRecord& c, CudaTimeRecord& time_rec, CudaTimeRecord& time_in_rec, CudaTimeRecord& time_out_rec) {
  c.Start();
  ScanChooseCroPoint_C();
  c.Record();

  for (int i = 0; i < pop_num; ++i) {
    for (int j = 0; j < test_num; ++j) {
      t_gene_map[i * test_num + j] = pop_vec[i].gene_code_vec[j];
    }
  }

  for (int i = 0; i < pop_num; ++i) {
    t_father_vec[i] = father_sel_vec[i];
  }


  time_in_rec.Start();
  cudaGetErrorString(cudaMemcpy(c_gene_map, t_gene_map, pop_num * test_num * sizeof(int), 
                                cudaMemcpyHostToDevice));
  cudaGetErrorString(cudaMemcpy(c_pos_map, t_pos_map, pop_num * test_num * sizeof(int), 
                                cudaMemcpyHostToDevice));
  cudaGetErrorString(cudaMemcpy(c_father_vec, t_father_vec, pop_num * sizeof(int), 
                                cudaMemcpyHostToDevice));
  time_in_rec.Record();
  //  time_out_rec.Record();

  cout << "ScanCross kernel start" << endl;
  time_rec.Start();
  CCudaCross<<<pop_num, MAXTHREAD>>>(c_gene_map, c_pos_map, c_father_vec, test_num, pop_num);
  time_rec.Record();

  cout << "ScanCross kernel end" << endl;

  for (int i = 0; i < pop_num * 2; ++i) {
    for (int j = 0; j < test_num; ++j) {
      t_gene_map[i * test_num + j] = 0;
    }
  }

  // Copy result out.
  time_out_rec.Start();
  cudaGetErrorString(cudaMemcpy(t_gene_map, c_gene_map,
    				pop_num * test_num * 2 * sizeof(int), cudaMemcpyDeviceToHost));
  time_out_rec.Record();


  /*
  for (int i = 0; i < pop_num; ++i) {
    int father = t_father_vec[i];
    for (int j = 0; j < test_num; ++j) {
      cout << pop_vec[father].gene_code_vec[j] << " ";
    }
    cout << endl;
    for (int j = 0; j < test_num; ++j) {
      cout << t_gene_map[(pop_num + i) * test_num + j] << " ";
    }
    cout << endl;
    cout << endl;
    getchar();
  }

  for (int i = 0; i < pop_num * 2; ++i) {
    int count = 0;

    for (int j = 0; j < test_num; ++j) {
      cout << t_gene_map[i * test_num + j] << " ";
      count += t_gene_map[i * test_num + j];
    }

    cout << "count: " << count << endl;
    cout << endl;
    getchar();
  }
*/
  for (int i = 0; i < pop_num * 2; ++i) {
    for (int j = 0; j < test_num; ++j) {
      pop_vec[i].gene_code_vec[j] = t_gene_map[i * test_num + j];
    }
  }
}
