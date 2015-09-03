#include <iostream>

#include "gene.h"
#include "initdata.h"
#include "log_front0.h"

void LogFront0(TimeRecord& c) {
  int pos = 0;
  while (pos < pop_num) {
    if (pop_vec[pos].front_num != 0) {
      break;
    }
    char str[256];
    sprintf(str, "%lf %lf\n", pop_vec[pos].apsc, pop_vec[pos].eff_time);
    c.Comment(str);
    ++pos;
  }
}
