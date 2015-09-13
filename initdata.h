#ifndef INIT_H_
#define INIT_H_

#include "conf.h"
#include "time_record.h"
#include <list>

#define FILE_NAME_LEN 256
#define DEFAULT_VALUE -1
// the initialize vector size and the expand size
#define VEC_SIZE 128
#define CMD_LEN 256
#define MAX_NUM 1000000000
#define MIN_NUM -1000000000
#define STABLE_COUNT 5
#define NOEXIST -1

// the iteration of the algorithm.
extern int ite_num;

// Generation information
extern int gen_num;
extern int pop_num;
extern int test_num;
extern int code_num;
extern double cross_propo;
extern double impro_bound;

// Input files.
extern string cov_file;
extern string time_file;

extern int cur_ite_num;
extern int cur_gen_num;

// flags to judge whether the algorithm is stable. 
extern double best_eet;
extern double best_apsc;
// The number of successive stable iterations.
extern int stable_count;

// Output files
extern string out_dir;
extern string cur_gen_dir;

extern string initialize_data_file;
extern string first_com_fit_file;
extern string initialize_pop_file;

extern string fatherselect_file;
extern string crossover_file;
extern string mutaupdatefitness_file;
extern string croupdatefitness_file;
extern string scroupdatefitness_file;
extern string cuda_crossover_file;
extern string cuda_scroupdatefitness_file;
extern string cuda_smutaupdatefitness_file;
extern string cuda_init_fitness_file;
extern string cuda_computefitness_file;
extern string computefitness_file;
extern string mutation_file;
extern string eliteselect_file;
extern string generation_file;
extern string front0_file;

extern vector<int> father_sel_vec;
extern vector<int> swap_vec_a;
extern vector<int> swap_vec_b;
extern vector<int> single_rand_vec;

extern list<list<int> > front;

int GetConf(string conf_file);

// @description: initialize the population
int InitData();
//int InitData(string conf_file);

#endif // INIT_H_
