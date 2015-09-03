#ifndef CUDA_INITDATA_H_
#define CUDA_INITDATA_H_

#include "time_record.h"

#define CUDA_CALL(x) do { if((x) != cudaSuccess) {\
    printf("CUDA Error at %s:%s\n",__FILE__,__LINE__);\
    exit(EXIT_FAILURE);}\
    } while(0)

#define CURAND_CALL(x) do { if((x) != CURAND_STATUS_SUCCESS) {\
    printf("CURAND Error at %s:%d\n",__FILE__,__LINE__);\
    exit(EXIT_FAILURE);}\
    } while(0)

#define MAXTHREAD 512
// #define CODE_NUM 125328
// #define CODE_NUM 11876
// #define CODE_NUM 512
// #define TEST_NUM 9286
// #define TEST_NUM 9282
// #define TEST_NUM 512
// #define POP_NUM 4

// Var in GPU.
extern float* c_time_vec;
extern int* c_gene_map;
extern int* c_first_exe_map;
extern int *c_rand_vec;
extern int *c_father_vec;
extern float *c_fitness_vec;
extern int* c_single_pos_map;
extern int* c_swap_vec;
extern char* c_cover_map;
extern int* c_pos_map;

// Temp var.
extern float * t_time_vec;
extern int* t_gene_map;
extern int* t_first_exe_map;
//  int *t_rand_vec = new int[gen_num * (pop_num >> 1)];
extern int *t_rand_vec;
extern int *t_father_vec;
extern float* t_fitness_vec;
extern int* t_single_pos_map;
extern int* t_swap_vec;
extern char* t_cover_map;
extern int* t_pos_map;

int CudaInitData(TimeRecord& c);

void CudaDeleteData();

#endif // CUDA_INITDATA_H_
