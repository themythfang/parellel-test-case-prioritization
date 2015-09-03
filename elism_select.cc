#include "elism_select.h"
#include "initdata.h"
#include "gene.h"
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <list>

using namespace std;

struct CrowdDis {
  // The crowding distance of the individual.
  double dis;
  // The code that this individual in the population.
  int pop_code;
};

bool op(const CrowdDis& c1, const CrowdDis& c2) {
  return c1.dis > c2.dis;
}


void ElismSelect() {

  list<int>::iterator it;
  list<list<int> >::iterator f_it;
  list<list<int> >::iterator c_it;

  vector<int> t_pop_vec;
  t_pop_vec.resize(pop_num);

  int pos = 0;
  for (f_it = front.begin(); f_it != front.end(); ++f_it) {
    //    pos += f_it->size();
    if (pos + f_it->size() > pop_num) {
      c_it = f_it;
      break;
    } else {
      for (it = f_it->begin(); it != f_it->end(); ++it) {
        t_pop_vec[pos] = *it;
        ++pos;
      }
    }
  }

  // Store the number of the computed crowding distances.
  list<CrowdDis> dis_list;
  list<double>::iterator double_it;
  list<CrowdDis>::iterator dis_it;
  //  list<double> apsc_sort_list;
  //  list<double> eff_sort_list;
  if (pos != pop_num) {
    // Choose the individuals according to the crowding distances
    // to maximize the population diversity.
    //    apsc_sort_list.clear();
    //    eff_sort_list.clear();
  list<double> apsc_sort_list;
  list<double> eff_sort_list;
    //    apsc_sort_list = list<double>();
    //    eff_sort_list = list<double>();
    for (int i = 0; i < (pop_num << 1); ++i) {
      apsc_sort_list.push_back(pop_vec[i].apsc);
      eff_sort_list.push_back(pop_vec[i].eff_time);
    }
    apsc_sort_list.sort();
    eff_sort_list.sort();

    for (it = c_it->begin(); it != c_it->end(); ++it) {
      CrowdDis cd;
      cd.pop_code = *it;
      for (double_it = apsc_sort_list.begin(); double_it != apsc_sort_list.end(); ++double_it) {
        if (*double_it == pop_vec[*it].apsc) {
          break;
        }
      }
      if(double_it == apsc_sort_list.begin() || 
         double_it == (--apsc_sort_list.end())) {
        cd.dis = MAX_NUM;
      } else {
        cd.dis = (*(--double_it) + *(++double_it));
        for (double_it = eff_sort_list.begin(); double_it != eff_sort_list.end(); ++double_it) {
          if (*double_it == pop_vec[*it].eff_time) {
            break;
          }
        }
        if (double_it == eff_sort_list.begin() || 
            double_it == (--eff_sort_list.end())) {
          cd.dis = MAX_NUM;
        } else {
          cd.dis += (*(--double_it) + *(++double_it));
        }
      }
      dis_list.push_back(cd);
    }
    dis_list.sort(op);
    dis_it = dis_list.begin();
    while(pos != pop_num) {
      t_pop_vec[pos++] = dis_it->pop_code;
      ++dis_it;
    }
  }

  cout << "pos: " << pos << endl;
  vector<int> flags_vec;
  flags_vec.resize(pop_num << 1);
  for (int i = 0; i < pop_num; ++i) {
    flags_vec[t_pop_vec[i]] = 1;
  }
  /*
  int posa, posb;
  posa = posb = 0;
  while (posb < (pop_num << 1)) {
    if (flags_vec[posb] == 1) {
      pop_vec[posa] = Individual(pop_vec[posb]);
      ++posa;
    }
    ++posb;
  }
  */
  //  vector<Individual> temp_pop_vec;
  //  temp_pop_vec.resize(pop_num);
  // Choose the next generation.
  for (int i = 0 ; i < pop_num; ++i) {
    temp_pop_vec[i] = Individual(pop_vec[t_pop_vec[i]]);
  }

  for (int i = 0; i < pop_num; ++i) {
    pop_vec[i] = Individual(temp_pop_vec[i]);
  }
}
