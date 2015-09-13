#include "elism_select.h"
#include "initdata.h"
#include "gene.h"
#include "non_dom_sort.h"
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <list>
#include <map>

using namespace std;

// Compute all elems' crowding distance in their front.
// This should be invoked after NonDomSort();
void CompCrowdDis(int pos, int num) {

  multimap<double, int> apsc_multimap;
  multimap<double, int> eff_time_multimap;
  multimap<double, int>::iterator apsc_it;
  multimap<double, int>::iterator eff_time_it;

  vector<double> dis_vec;
  dis_vec.resize(pos + num);
  dis_vec.assign(pos + num, 0);

  list<list<int> >::iterator f_it = front.begin(); 
  while (f_it != front.end()) {
    // Choose the individuals according to the crowding distances
    // to maximize the population diversity.
    list<double> apsc_sort_list;
    list<double> eff_sort_list;
    list<int>::iterator p_it;

    apsc_multimap.clear();
    eff_time_multimap.clear();
    // sort every objective.
    for (p_it = f_it->begin(); p_it != f_it->end(); ++p_it) {
      int t_id = *p_it;
      apsc_multimap.insert(pair<double, int>(pop_vec[t_id].apsc, t_id));
      eff_time_multimap.insert(pair<double, int>(pop_vec[t_id].eff_time, t_id));
    }

    // store map into vector for easier usage.
    // maybe redundancy, their is better implement,
    // but who cares.
    apsc_it = apsc_multimap.begin();
    eff_time_it = eff_time_multimap.begin();
    vector<pair<double, int> > apsc_vec;
    vector<pair<double, int> > eff_time_vec;
    for (int i = 0; i < f_it->size(); ++i) {
      apsc_vec.push_back(pair<double, int>(apsc_it->first, apsc_it->second));
      eff_time_vec.push_back(pair<double, int>(eff_time_it->first, eff_time_it->second));
      ++apsc_it;
      ++eff_time_it;
    }

    // Compute the crowding-distance.
    for (int i = 0; i < apsc_vec.size(); ++i) {
      if ((apsc_vec[i].second < 0 || apsc_vec[i].second >= num) || 
          (eff_time_vec[i].second < 0 || eff_time_vec[i].second >= num)) {
        cout << "Wrong" << endl;
        getchar();
      }

      if (i == 0 || i == apsc_vec.size() - 1) {
        dis_vec[apsc_vec[i].second] = MAX_NUM; 
        dis_vec[eff_time_vec[i].second] = MAX_NUM;
      } else {
        if (dis_vec[apsc_vec[i].second] != MAX_NUM) {
          dis_vec[apsc_vec[i].second] += apsc_vec[i + 1].first - apsc_vec[i - 1].first;
        }
        if (dis_vec[eff_time_vec[i].second] != MAX_NUM) {
          dis_vec[eff_time_vec[i].second] += eff_time_vec[i + 1].first - eff_time_vec[i - 1].first;
        }
      }
    }
    ++f_it;
  }

  for (int i = 0; i < num; ++i) {
    pop_vec[i].crowd_dis = dis_vec[i];
  }
}

void ElismSelect() {
  int pos = 0;
  int num = pop_num * 2;

  cout << "non" << endl;
  NonDomSort(pos, num);
  cout << "comp" << endl;
  CompCrowdDis(pos, num);

  cout << "start" << endl;
  int target_num = pop_num;
  vector<int> t_pop_vec;

  list<list<int> >::iterator f_it = front.begin();
  list<int>::iterator it = f_it->begin();
  //  for (f_it = front.begin(); f_it != front.end(); ++f_it) {
  while (f_it != front.end()) {
    if (t_pop_vec.size() + f_it->size() > target_num) {
      break;
    } else {
      it = f_it->begin();
      while (it != f_it->end()) {
        t_pop_vec.push_back(*it);
        ++it;
      }
    }
    ++f_it;
  }

  if (t_pop_vec.size() != target_num) {
    multimap<double, int, greater<double> > dis_map;
    it = f_it->begin();
    while (it != f_it->end()) {
      dis_map.insert(pair<double, int>(pop_vec[*it].crowd_dis, *it));
      ++it;
    }
    
    multimap<double, int, greater<double> >::iterator dis_map_it = dis_map.begin();
    while (dis_map_it != dis_map.end() && t_pop_vec.size() < target_num) {
      t_pop_vec.push_back(dis_map_it->second);
      ++dis_map_it;
    }
  }

  for (int i = 0; i < target_num; ++i) {
    if (t_pop_vec[i] > num) {
      cout << i << " " << t_pop_vec[i] << endl;
      cout << "Wrong!" << endl;
      getchar();
    }
  }

  // Choose the next generation.
  for (int i = 0 ; i < target_num; ++i) {
    temp_pop_vec[i] = pop_vec[t_pop_vec[i]];
  }

  for (int i = 0; i < target_num; ++i) {
    pop_vec[pos + i] = Individual(temp_pop_vec[i]);
  }
}
