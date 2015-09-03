#include "time_record.h"
#include <fstream>
#include<sys/time.h>

void TimeRecord::Start() {
  struct  timeval    tv;
  if (gettimeofday(&tv, NULL) == 0) {
    start_time = (tv.tv_sec * 1000000 + tv.tv_usec) * 1.0 / 1000000;
  } 
}

void TimeRecord::Comment(string com) {
  ofstream rec_fp;
  if (OpenFile(rec_file_, rec_fp) == 0) {
    rec_fp << com;
    rec_fp.close();
  }
}

void TimeRecord::Record() {
  struct  timeval    tv;
  if (gettimeofday(&tv, NULL) == 0) {
    end_time = (tv.tv_sec * 1000000 + tv.tv_usec) * 1.0 / 1000000;
  }
  dur_time = end_time - start_time;
  ofstream rec_fp;
  if (OpenFile(rec_file_, rec_fp) == 0) {
    rec_fp << dur_time << endl;
    rec_fp.close();
  }
}

int TimeRecord::OpenFile(string file_name, ofstream& fp) {
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
