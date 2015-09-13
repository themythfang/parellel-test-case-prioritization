#include <iostream>
#include <math.h>

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
  c.Comment("\n");
}

// Judge whether the algorithm is stable
// return 0 for unstable
// return 1 for stable.
int IsStable() {
  /*
  for (int i = 0; i < pop_num * 2; ++i) {
    cout << i << " " << pop_vec[i].front_num << " " << pop_vec[i].apsc << " " << pop_vec[i].eff_time << endl;

  }
  getchar();
  */

  int pos = 0;
  double cur_best_apsc = MIN_NUM;
  double cur_best_eet = MAX_NUM;
  while (pos < pop_num) {
    cout << pop_vec[pos].apsc << " " << pop_vec[pos].eff_time << endl;
    if (pop_vec[pos].front_num != 0) {
      break;
    }
    if (pop_vec[pos].apsc > cur_best_apsc) {
      cur_best_apsc = pop_vec[pos].apsc;
    }

    if (pop_vec[pos].eff_time < cur_best_eet) {
      cur_best_eet = pop_vec[pos].eff_time;
    }
    ++pos;
  }

  cout << "Ite: " << ite_num << " " << cur_best_apsc << " " << cur_best_eet << endl;
  cout << "Ite: " << ite_num << " " << best_apsc << " " << best_eet << endl;
  if (fabs(cur_best_apsc - best_apsc) < 0.005 &&
      fabs(cur_best_eet - best_eet) < 1.0) {
    ++stable_count;
    if (stable_count >= STABLE_COUNT) {
      return 1;
    }
  } else {
    stable_count = 0;
  }

  best_apsc = cur_best_apsc;
  best_eet = cur_best_eet;
  return 0;
}
