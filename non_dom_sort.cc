#include "initdata.h"
#include "gene.h"
#include "non_dom_sort.h"
#include "time_record.h"
#include <list>
#include <fstream>
using namespace std;

// @description: non-dominate sort algorithm
//  addressed by 
// @author: Kalyanmoy Deb
// @title: in A Fast Elitist Non-Dominated Sorting Genetic
// Algorithm for Multi-Objective Optimization: NSGA-II
int NonDomSort(int pos, int num) {
  // statistics the domination number of the individual.

  cout << "Nondom 1" << endl;
  for (int i = pos; i < num; ++i) {
    pop_vec[i].dom_num = 0;
    pop_vec[i].dom_list.resize(0);
  }
  cout << "Nondom" << 2 << endl;

  for (int i = pos; i < pos + num; ++i) {
    for (int j = pos; j < pos + num; ++j) {
      if (j != i) {
        if (pop_vec[i].apsc > pop_vec[j].apsc &&
            pop_vec[i].eff_time < pop_vec[j].eff_time) {
          ++pop_vec[j].dom_num;
          pop_vec[i].dom_list.push_back(j);
        }
      }
    }
  }

  cout << "nomdom 3 " << endl;

  list<int>::iterator it;

  // Store the individuals that aren't sorted by nsga-2.
  list<int> indi_list;
  indi_list.clear();
  for (int i = pos; i < pos + num; ++i) {
    indi_list.push_back(i);
  }
  // Current top front.
  // consist of individuals that aren't dominated by any individuals
  // in &indi_list.
  list<int> cur_top_front;

  list<list<int> >::iterator f_it;
  list<int>::iterator c_it;
  list<int>::iterator p_it;
  list<int>::iterator d_it;

  // Initialize the front.

  for (f_it = front.begin(); f_it != front.end(); ++f_it) {
    f_it->clear();
  }

  front.clear();

  while(indi_list.size() > 0) {
    //  for (int i = 0; i < 10; ++i) {
    //    cout << indi_list.size() << " " << cur_top_front.size() << endl;
    cur_top_front.clear();

    p_it = indi_list.begin();
    while(p_it != indi_list.end()) {
      if(pop_vec[*p_it].dom_num == 0) {
        // Store the front number of current individual.
        pop_vec[*p_it].front_num = front.size();

        cur_top_front.push_back(*p_it);
        indi_list.erase(p_it);
        p_it = indi_list.begin();
      } else {
        ++p_it;
      }
    }
    // Create a new front.
    list<int> c = cur_top_front;
    front.push_back(c);

    c_it = cur_top_front.begin();
    while(c_it != cur_top_front.end()) {
      d_it = pop_vec[*c_it].dom_list.begin();
      while(d_it != pop_vec[*c_it].dom_list.end()) {
        --pop_vec[*d_it].dom_num;
        ++d_it;
      }
      ++c_it;
    }
  }
  cout << "non dom sort 3" << endl;

  /*
  ofstream front0_fp((out_dir + cur_gen_dir + front0_file).c_str(), ios::app);

  f_it = front.begin();
  front0_fp << cur_gen_num << ":" << endl;
  for (c_it = f_it->begin(); c_it != f_it->end(); ++c_it) {
    front0_fp << pop_vec[*c_it].apsc << " " << pop_vec[*c_it].eff_time << endl;
    //    fprintf(front0_fp, "%lf %lf\n", pop_vec[*c_it].apsc, pop_vec[*c_it].eff_time);
  }
  front0_fp << endl << endl;;
  front0_fp.close();
  */

  cout << "non end!" << endl;
  return 0;
}
