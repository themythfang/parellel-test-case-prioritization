#ifndef CUDA_TIME_RECORD_H
#define CUDA_TIME_RECORD_H

#include <iostream>
#include <fstream>
#include <cuda.h>
#include <cuda_runtime.h>
#include <cuda_runtime_api.h>
#include <device_functions.h>
#include <device_launch_parameters.h>

using namespace std;

/*
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)
*/

// class CudaTimeRecord {
class CudaTimeRecord {
 public:
  CudaTimeRecord(string rec_file);

  void Start();

  void Comment(string com);

  void Record();

 private:
  int OpenFile(string file_name, ofstream& stream);
  // the record file.

  cudaEvent_t timeStartEvent_; 
  cudaEvent_t timeEndEvent_;
  string rec_file_;
  // the duration time.
  float dur_time_;
  bool is_first_open_;
};

#endif //  CUDA_TIME_RECORD_H
