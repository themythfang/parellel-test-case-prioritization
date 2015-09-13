#include "initdata.h"
#include "initpop.h"
#include "gene.h"
#include <istream>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

int ite_num = 1;

// Generation information
int gen_num = 100;
int pop_num = 128;
int test_num = DEFAULT_VALUE;
int code_num = DEFAULT_VALUE;
double scan_propo = 0.1;
double cross_propo = 0.1;
double mutation_propo = 0.001;
double impro_bound = 0.05;

// The input file.
string cov_file;
string time_file;

int cur_ite_num;
int cur_gen_num;

int code_num_set = 0;
int test_num_set = 0;

double best_eet = -1;
double best_apsc = -1;
int stable_count = 0;

// The output directory.
// The final output file will be the format
// $out_dir + $cur_gen_dir + file_name
string out_dir = "default_data//";
string cur_gen_dir;
string initialize_data_file = "initialize_data_log";
string initialize_fit_file = "initialize_fit_log";
string first_com_fit_file = "first_com_fit_log";
string initialize_pop_file = "initialize_pop_log"; 
string fatherselect_file = "fatherselect_log";
string crossover_file = "crossover_log";
string mutaupdatefitness_file = "mutaupdatefitness_log";

/*
string scancroupdatefitness_file = "scancroupdatefitness_log";
string rscancroupdatefitness_file = "rscancroupdatefitness_log";
string singlecroupdatefitness_file = "singlecroupdatefitness_log";
string reverse_singlecroupdatefitness_file = "reverse_singlecroupdatefitness_log";
string ordercroupdatefitness_file = "ordercroupdatefitness_log";
string cuda_scroupdatefitness_file = "cuda_scroupdatefitness_log";
string cuda_smutaupdatefitness_file = "cuda_smutaupdatefitness_log";
*/

string cuda_init_fitness_file = "cuda_initfitness_log";;
string cuda_crossover_file = "cuda_crossover_log";
string cuda_computefitness_file = "cuda_comp_fitness_log";

string computefitness_file = "computefitness_log";
string mutation_file = "mutation_log";
string eliteselect_file = "eliteselect_log";
string generation_file = "generation_log";
string front0_file = "front0";
string crossover_tc_file = "crossover_tc";
string mutation_tc_file = "mutation_tc";

vector<int> father_sel_vec;
vector<int> swap_vec_a;
vector<int> swap_vec_b;
vector<int> single_rand_vec;
vector<int> pop_id;
vector<int> first_exe_vec;
vector<int> muta_vec;

list<list<int> > front;
vector<pair<double, double> > front0_value;

// @description: get the integer value of $sec_name.$key_name from file $conf_file
// @return
// 0 success
// -1 configuration tag lost.
int ReadConfToInt(string conf_file, string sec_name, string key_name, int& value) {
  string t_str = CIceIniFile::GetValue(key_name, sec_name, conf_file);
  if (t_str.length() == 0) {
    cerr << "Failed to get integer value of " << sec_name << "." << key_name << " from configuratoin file " << conf_file << endl;
    return -1;
  }
  value = atoi(t_str.c_str());
  return 0;
}

// @description: get the double value of $sec_name.$key_name from file $conf_file
// @return
// 0 success
// -1 configuration tag lost.
int ReadConfToDouble(string conf_file, string sec_name, string key_name, double& value) {
  string t_str = CIceIniFile::GetValue(key_name, sec_name, conf_file);
  if (t_str.length() == 0) {
    cerr << "Failed to get double value of " << sec_name << "." << key_name << " from configuratoin file " << conf_file << endl;
    return -1;
  }
  value = atof(t_str.c_str());
  return 0;
}

int ReadConfToString(string conf_file, string sec_name, string key_name, string& str) {
  str = CIceIniFile::GetValue(key_name, sec_name, conf_file);
  if (str.length() == 0) {
    cerr << "Failed to get string value of " << sec_name << "." << key_name << " from configuratoin file " << conf_file << endl;
    return -1;
  }
  return 0;
}

// @description: get the configuration of the algorithm
// @return
// 0 success
// -1 configuration tag lost.
int GetConf(string conf_file) {
  if (ReadConfToInt(conf_file, "nsga2", "iteration_num" ,ite_num) == -1) {
    return -1;
  }

  if (ReadConfToInt(conf_file, "nsga2", "generation_num" ,gen_num) == -1) {
    return -1;
  }
  if (ReadConfToInt(conf_file, "nsga2", "population_num" ,pop_num) == -1) {
    return -1;
  }
  // not set
  test_num_set = 0;
  if (ReadConfToInt(conf_file, "nsga2", "test_num" , test_num) == -1) {
    //    return -1;
  } else {
    test_num_set = 1;
  }

  // not set
  code_num_set = 0;
  if (ReadConfToInt(conf_file, "nsga2", "code_num" , code_num) == -1) {
    //    return -1;
  } else {
    code_num_set = 1;
  }

  if (ReadConfToDouble(conf_file, "nsga2", "cross_propo" ,cross_propo) == -1) {
  }
  if (ReadConfToDouble(conf_file, "nsga2", "scan_propo" ,scan_propo) == -1) {
  }
  if (ReadConfToDouble(conf_file, "nsga2", "mutation_propo", mutation_propo) == -1) {
  }
  if (ReadConfToDouble(conf_file, "nsga2", "impro_bound", impro_bound) == -1) {
    return -1;
  }
  if (ReadConfToString(conf_file, "data", "time", time_file) == -1) {
    return -1;
  }
  if ( ReadConfToString(conf_file, "data", "coverage", cov_file) == -1) {
    return -1;
  }
  return 0;
}

// @description: read data from file $cov_file & $time_file
// @return
// 0 success
// -1 failed to open file
// -2 coverage file is empty
// -3 time file's data doesn't coverage file

int ReadData() {
  ifstream cov_fp(cov_file.c_str());
  ifstream time_fp(time_file.c_str());
  struct stat info;

  string t_str;
  int t_test_num = 0;

  //  if ((cov_fp = fopen(cov_file.c_str(), "r")) == NULL) {
  cout << "access: " << access(cov_file.c_str(), F_OK) << endl;

  stat(cov_file.c_str() ,&info);
  if (S_ISDIR(info.st_mode) || !cov_fp.is_open()) {
    cerr << "Open " << cov_file << " error!" << endl;
    return -1;
  } else {
    // get sample line to set $code_num
    //    if (fgets(buf, CODE_BUFFER_LEN, cov_fp) != NULL) {
    gen_vec.resize(VEC_SIZE);
    cout << cov_file << endl;
    getchar();

    t_test_num = 0;
    while (!std::getline(cov_fp, gen_vec[t_test_num].coverage).eof()) {
      ++t_test_num;
      if (t_test_num >= gen_vec.size()) {
        gen_vec.resize(gen_vec.size() + VEC_SIZE);
      }

      if (test_num_set == 1 && t_test_num == test_num) {
      //      if (test_num != DEFAULT_VALUE && t_test_num == test_num) {
        break;
      }
    }
  }

  if (t_test_num > 0) {
    //    if (test_num == DEFAULT_VALUE) {
    if (test_num_set == 0) {
      test_num = t_test_num;
    }
    //    if (code_num == DEFAULT_VALUE) {
    if (code_num_set == 0) {
      code_num = gen_vec[0].coverage.length();
    } else if (code_num_set == 1 && code_num > gen_vec[0].coverage.length()) {
      cout << "code_NUM: " << code_num << endl;
      return -2;
    }

    father_sel_vec.resize(pop_num);
    swap_vec_a.resize(pop_num << 1);
    swap_vec_b.resize(pop_num << 1);
    single_rand_vec.resize(pop_num);
  } else {
    cout << "t_test_num: " << t_test_num << endl;
    return -3;
  }
  gen_vec.resize(t_test_num);

  cout << "code_num is: " << code_num << endl;
  cout << "test_num is: " << test_num << endl;
  cout << "GetData 2" << endl;

  stat(cov_file.c_str() ,&info);
  //  if (!time_fp.is_open()) {
  if (S_ISDIR(info.st_mode) || !time_fp.is_open()) {
    cerr << "OPEN " << time_file << " error!" << endl;
    return -1;
  } else {
    for (int i = 0; i < test_num; ++i) {
      if (std::getline(time_fp, t_str).eof()) {
        return -4;
      }
      gen_vec[i].time = atof(t_str.c_str());
      //      cout << gen_vec[i].time << " " << t_str << endl;
    }
    //    getchar();
  }
  cov_fp.close();
  time_fp.close();
  return 0;
}

// @description: initialize the data nsga2 need
// @return
// 0 success
// -1 failed to read data
int InitData() {
  int ret = 0;
  if ((ret = ReadData()) != 0) {
    cout << "ReadData Error " << ret << endl;
    return -1;
  }

  //  out_dir += "//";
  system(("mkdir " + out_dir).c_str());

  srand(time(NULL));
  return 0;
}
