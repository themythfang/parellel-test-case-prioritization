#include "cuda_time_record.h"
#include <fstream>
#include <cuda.h>
#include<sys/time.h>

CudaTimeRecord::CudaTimeRecord(string rec_file) :
  rec_file_(rec_file), is_first_open_(true) {
  cudaEventCreate(&timeStartEvent_, 0);
  cudaEventCreate(&timeEndEvent_, 0);
}

void CudaTimeRecord::Start() {
  cudaEventRecord(timeStartEvent_, 0);
}

void CudaTimeRecord::Comment(string com) {
  ofstream rec_fp;
  if (OpenFile(rec_file_, rec_fp) == 0) {
    rec_fp << com;
    rec_fp.close();
  }
}

void CudaTimeRecord::Record() {
  cudaEventRecord(timeEndEvent_, 0);
  cudaEventSynchronize(timeEndEvent_);
  cudaEventElapsedTime(&dur_time_,timeStartEvent_,timeEndEvent_);

  ofstream rec_fp;
  if (OpenFile(rec_file_, rec_fp) == 0) {
    rec_fp << dur_time_ << endl;
    rec_fp.close();
  }
}

int CudaTimeRecord::OpenFile(string file_name, ofstream& fp) {
  if (is_first_open_) {
    //  if (is_first_open_) {
    // Any contents that existed in the file before it is open are discarded.
    fp.open(file_name.c_str(), ios::trunc);
    is_first_open_ = false;
    cout << "if " << 1 << endl;
  } else {
    fp.open(file_name.c_str(), ios::app);
  }

  if (!fp.is_open()) {
    return -1;
  }

  return 0;
}
