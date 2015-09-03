#ifndef GENE_H_
#define GENE_H_

#include <string>
#include <vector>
#include <list>
#include <iostream>
using namespace std;

struct Gene {
 public:
  // The detailed coverage data.
  // Use char* instead of string to control memory use more accurately.
  //  char *coverage;
  string coverage;
  // The time of running the test case.
  double time;
};
extern vector<Gene> gen_vec;

// The individual of the nsga2 algorithm.
// $inter_pop_list store the intermediate results.
// $pop_list store the information of the generation.
struct Individual {
 public:
  // Store the gene of the individual.
  vector<int> gene_code_vec;
  // The ASPC
  double apsc;
  // The effective executing time of the individual.
  double eff_time;
  // The individual iteelf dominates.
  list<int> dom_list;
  // The number that dominates this individual.
  int dom_num;
  // The front number of this individual.
  int front_num;
  // The min block that need to execute.
  vector<int> first_exe_vec;
  // The struct that "SCAN ALGORITHM" uses.
  vector<int> pos_vec;
  // #ifdef OPTIMISE
  vector<int> flags_vec;
  vector<int> single_pos_vec;
  // The code of its father.
  int father;
  int mother;

  int total_sta;
};
// inter_pop_list[POP_NUM], pop_list[POP_NUM * 2];
extern vector<Individual> pop_vec;
extern vector<Individual> temp_pop_vec;

#endif // GENE_H_
