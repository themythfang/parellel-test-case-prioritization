#include "father_select.h"
#include "initdata.h"
#include "gene.h"
#include "non_dom_sort.h"
#include "elism_select.h"
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

    cout << "rand: " << rand << endl;

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


int CrowdCompare(const Individual& p1, const Individual& p2) {
  //  return (p1.front != p2.front ? p1.front < p2.front : p1.dis < p2.dis);
  if (p1.front_num == p2.front_num) {
    if (p1.crowd_dis > p2.crowd_dis) {
      return -1;
    } else {
      return 1;
    }
  } else if (p1.front_num < p2.front_num) {
    return -1;
  } else {
    return 1;
  }
}

void TourSelect(int num) {
  NonDomSort(0, num);
  CompCrowdDis(0, num);

  for (int i = 0; i < num; ++i) {
    int a = random() % pop_num;
    int b = random() % pop_num;
    while (b == a) {
      b = random() % pop_num;
    }
    if (CrowdCompare(pop_vec[a], pop_vec[b]) == -1) {
      father_sel_vec[i] = a;
    } else {
      father_sel_vec[i] = b;
    }
  }
}

