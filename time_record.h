#ifndef TIME_RECORD_H
#define TIME_RECORD_H

#include <iostream>
#include <fstream>
using namespace std;

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

class TimeRecord {
 public:
 TimeRecord(string rec_file) :
  rec_file_(rec_file), is_first_open_(true) {
  }

  void Start();

  void Comment(string com);

  void Record();

 private:
  DISALLOW_COPY_AND_ASSIGN(TimeRecord);
  int OpenFile(string file_name, ofstream& stream);
  // the record file.
  string rec_file_;
  double start_time;
  double end_time;
  // the duration time.
  double dur_time;
  bool is_first_open_;
};

#endif //  TIME_RECORD_H
