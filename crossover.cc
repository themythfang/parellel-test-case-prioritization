#include "crossover.h"
#include "father_select.h"
#include "initdata.h"
#include "gene.h"
#include "time_record.h"
#include <vector>
#include <stdlib.h>
#include <time.h>
using namespace std;

void ScanChooseCroPoint() {
  vector<int> flags;
  vector<int> pos;
  flags.resize(test_num);
  pos.resize(test_num);

  int t;

  // Set the random seed.
  int scan_num = test_num * cross_propo;

  for (int i = 0; i < pop_num; ++i) {
    // Initialize the vector with value 0
    flags.assign(test_num, 0);
    for (int j = 0; j < scan_num; ++j) {
      t = rand() % test_num;
      while (flags[t] == 1) {
	t = rand() % test_num;
      }
      flags[t] = 1;
    }

    // Generation the new generation using the algorithm "SCAN".
    int count = 0;
    for (int j = 0; j < test_num; ++j) {
      pos[j] = count;
      if (flags[j] == 1) {
	++count;
      }
    }

    for (int j = 0; j < test_num; ++j) {
      pop_vec[pop_num + i].flags_vec[j] = pos[j];
    }

    for (int j = 0; j < test_num; ++j) {
      if (flags[j] == 0) {
	pos[j] = scan_num + j - pos[j];
      }
    }

    for (int j = 0; j < test_num; ++j) {
      pop_vec[pop_num + i].pos_vec[j] = pos[j];
    }
  }
}

// @description: Use the SCAN algorithm to cross the population.
void ScanCrossOver() {
  ScanChooseCroPoint();

  int t_pos;
  for (int i = 0; i < pop_num; ++i) {
    int father_gen = father_sel_vec[i];

    pop_vec[pop_num + i].father = father_gen;

    for (int j = 0; j < test_num; ++j) {
      t_pos = pop_vec[pop_num + i].pos_vec[j];
      pop_vec[pop_num + i].gene_code_vec[t_pos] = pop_vec[father_gen].gene_code_vec[j];
    }
    pop_vec[i].first_exe_vec.assign(0, test_num);
  }
}

void OrderGetSonGene(int p1, int p2, int s, int rand1, int rand2) {
  vector<int> flags_vec;
  flags_vec.resize(test_num);
  flags_vec.assign(test_num, 0);

  for (int i = rand1; i < rand2; ++i) {
    int gene_code = pop_vec[p1].gene_code_vec[i];
    pop_vec[s].gene_code_vec[i] = gene_code;
    flags_vec[gene_code] = 1;
  }

  int pos = 0;
  for (int i = 0; i < test_num; ++i) {
    int gene_code = pop_vec[p2].gene_code_vec[i];
    if (flags_vec[gene_code] == 0) {
      pop_vec[s].gene_code_vec[pos] = gene_code;
      ++pos;
      if (pos == rand1) {
        pos = rand2;
      }
    }
  }

  /*
  cout << rand1 << " " << rand2 << endl;
  int count = 0;
  for (int i = 0; i < test_num; ++i) {
    count += pop_vec[s].gene_code_vec[i];
    //    cout << pop_vec[s].gene_code_vec[i] << " ";
  }
  //  cout << endl;
  cout << (0 + test_num - 1) * test_num / 2 << " " << count << endl;
  getchar();
  */
}

void OrderCrossOver() {
  for (int i = 0; i < (pop_num >> 1); ++i) {
    int s1 = pop_num + i * 2;
    int s2 = s1 + 1;
    int p1 = father_sel_vec[i * 2];
    int p2 = father_sel_vec[i * 2 + 1];

    cout << "P: " << p1 << " " << p2 << endl;

    int rand1, rand2;
    rand1 = random() % test_num;
    rand2 = random() % test_num;
    while (rand2 == rand1) {
      rand2 = random() % test_num;
    }

    if (rand1 > rand2) {
      swap(rand1, rand2);
    }

    OrderGetSonGene(p1, p2, s1, rand1, rand2);
    OrderGetSonGene(p2, p1, s2, rand1, rand2);
  }
}

void GetSonGene(int p1, int p2, int s, int k) {
  int t_code;
  vector<int> flag_vec;
  flag_vec.resize(test_num);
  flag_vec.assign(test_num, 0);

  pop_vec[s].father = p1;
  pop_vec[s].mother = p2;
  for (int j = 0; j < k; ++j) {
    t_code = pop_vec[p1].gene_code_vec[j];
    pop_vec[s].gene_code_vec[j] = t_code;
    flag_vec[t_code] = 1;
  }
  int c = k;
  for (int j = 0; j < test_num; ++j) {
    // Judge the code can exist in the sequence legally.
    // If legally, add it, else pass it.
    int t_code = pop_vec[p2].gene_code_vec[j];
    if (flag_vec[t_code] == 0) {
      // If legally.
      pop_vec[s].gene_code_vec[c] = t_code;
      //      pos_vec[j] = c;
      pop_vec[s].single_pos_vec[j] = c;
      ++c;
    }
  }

}

// @description: Antoniol's Single Point Crossover
// generate (pop_num / 2) 
void SinglePointCrossover() {
  srand(time(NULL));
  // The position that crossover occurs.
  int k;
  // Hash table to label whether test case in the position is used.
  vector<int> flag_vec1;
  vector<int> flag_vec2;
  flag_vec1.resize(test_num);
  flag_vec2.resize(test_num);

  // useless, just to satify the interface of
  // function GetSonGene;

  // The father code.
  int p1, p2;
  // The son code.
  int s1, s2;

  for (int i = 0; i < (pop_num >> 1); ++i) {
    p1 = father_sel_vec[i << 1];
    p2 = father_sel_vec[i << 1 | 1];
    s1 = (i << 1) + pop_num;
    s2 = (i<< 1 | 1) + pop_num;

    // Choose one points randomly.
    k = random() % test_num;

    single_rand_vec[i] = k;

    GetSonGene(p1, p2, s1, k);
    GetSonGene(p2, p1, s2, k);
    pop_vec[s1].first_exe_vec.assign(0, test_num);
    pop_vec[s2].first_exe_vec.assign(0, test_num);
  }
}
