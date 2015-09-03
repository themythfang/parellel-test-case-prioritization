#ifndef CUDA_FITNESS_H_
#define CUDA_FITNESS_H_

#include "gene.h"
#include "initdata.h"
#include "cuda_time_record.h"
#include <cuda.h>

#define CUDA_CALL(x) do { if((x) != cudaSuccess) {\
    printf("CUDA Error at %s:%s\n",__FILE__,__LINE__);\
    exit(EXIT_FAILURE);}\
    } while(0)

#define CURAND_CALL(x) do { if((x) != CURAND_STATUS_SUCCESS) {\
    printf("CURAND Error at %s:%d\n",__FILE__,__LINE__);\
    exit(EXIT_FAILURE);}\
    } while(0)


void CudaCompFit(CudaTimeRecord& time_rec, int com_num);

void CudaCompFit2(CudaTimeRecord& time_rec, CudaTimeRecord& time_in_rec, CudaTimeRecord& time_out_rec, int com_num);

void CudaSinglePointCrossoverUpdFit(CudaTimeRecord& time_rec);
// void CudaSinglePointCrossoverUpdFit(TimeRecord& time_rec);

void CudaSwapMutationUpdFit(CudaTimeRecord& time_rec);
// void CudaSwapMutationUpdFit(TimeRecord& time_rec);


#endif // CUDA_FITNESS_H_
