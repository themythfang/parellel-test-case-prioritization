#ifndef CUDA_CROSSOVER_H_
#define CUDA_CROSSOVER_H_

#include "cuda_initdata.h"

#define DMAXTHREAD (MAXTHREAD << 1)

void CudaSingleCross(CudaTimeRecord& time_rec);

// void CudaScanCross(CudaTimeRecord& time_rec);

void CudaScanCross(TimeRecord& c, CudaTimeRecord& time_rec, CudaTimeRecord& time_in_rec, CudaTimeRecord& time_out_rec);

#endif // CUDA_CROSSOVER_H_
