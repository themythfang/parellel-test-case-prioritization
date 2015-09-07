#include "father_select.h"
#include "initdata.h"
#include "gene.h"
#include <stdlib.h>
#include <time.h>

void FatherSelect(int num) {
  father_sel_vec.resize(pop_num);
  // Initialize APSC fitness map
  double total_apsc = 0;
  vector<double> apsc_map;
  apsc_map.resize(pop_num);

  for (int i = 0; i < pop_num; ++i) {
    total_apsc += pop_vec[i].apsc;
  }
  apsc_map[0] = pop_vec[0].apsc / total_apsc;

  for (int i = 1; i < pop_num; ++i) {
    apsc_map[i] = apsc_map[i - 1] + pop_vec[i].apsc / total_apsc;
  }

  for (int i = 0; i < num; ++i) {
    double rand = random() * 1.0 / RAND_MAX;
    // in case of float number computation error
    father_sel_vec[i] = pop_num - 1;

    int flag = 0;
    for (int j = 0; j < pop_num; ++j) {
      if (rand < apsc_map[j]) {
	father_sel_vec[i] = j;
	break;
      }
    }
    if (flag == 0) {
      father_sel_vec[i] = pop_num - 1;
    }
  }
}
