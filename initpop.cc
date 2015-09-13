#include "initpop.h"
#include "initdata.h"
#include "fitness.h"
#include "gene.h"
#include <list>
#include <stdlib.h>
#include <time.h>
using namespace std;

void GenRandPop(int gen_pos, int num) {
  srand(time(NULL));
  // The the initialize data of gene code.
  list<int> gene_list;
  // Temp gene code.
  //  list<int> t_gene_list;

  for (int i = 0; i < test_num; ++i) {
    gene_list.push_back(i);
  }

  list<int>::iterator it;
  int rand_num;
  srand(time(NULL));
  // Generate gene code sequence randomly.
  for (int i = 0; i < num; ++i) {
    cout << "randomly generate " << i << endl;
    list<int> t_gene_list = gene_list;
    // t_gene_list = gene_list;
    for (int j = 0; j < test_num; ++j) {
      rand_num = rand() % (test_num - j);

      it = t_gene_list.begin();
      while(rand_num != 0) {
        --rand_num;
        ++it;
      }
      // Put the gene to the population list.
      pop_vec[gen_pos + i].gene_code_vec[j] = *it;

      // Remove the gene from the temp list.
      t_gene_list.erase(it);
    }
  }
}

void InitPop() {
  // The former $pop_num individuals constitute the father generation;
  // The latter $pop_num individuals constitute the son generation.

  cout << "pop_num is:" << pop_num << endl;

  pop_vec.resize(pop_num << 1);
  temp_pop_vec.resize(pop_num);
  for (int i = 0; i < (pop_num << 1); ++i) {
    pop_vec[i].gene_code_vec.resize(test_num);

    pop_vec[i].first_exe_vec.resize(code_num);
    // Assume that the last test case will cover this line of code.
    pop_vec[i].first_exe_vec.assign(code_num, test_num);
    pop_vec[i].pos_vec.resize(test_num);
    pop_vec[i].flags_vec.resize(test_num);
    //    pop_vec[i].single_pos_vec.resize(test_num);
  }
  cout << 2 << endl;
  GenRandPop(0, pop_num);
}

