// @author : fangyuan 
// @mail : themythfang@gmail.com
// @time : 2014.4.27
// This code is designed for NSGA2.
// References: 

#include "non_dom_sort.h"
#include "initdata.h"
#include "initpop.h"
#include "gene.h"
#include "father_select.h"
#include "crossover.h"
#include "mutation.h"
#include "fitness.h"
#include "log_front0.h"
#include "elism_select.h"
#include "time_record.h"
#include "cuda_initdata.h"
#include "cuda_fitness.h"
#include "cuda_crossover.h"

#include <iostream>
using namespace std;

#define CUDA_FITNESS
// #define CUDA_CROSS

int main() {
  string c_file = "nsga2.conf";

  if (GetConf(c_file) != 0) {
    return -1;
  }

  cout << "BATCH" << endl;
  string out_d = out_dir;
  string cov_dir = cov_file;
  string time_dir = time_file;
  char buf[128];
  system("mkdir default_data");
  for (int in = 0; in < 50; ++in) {
    sprintf(buf, "%sdata%d//", out_d.c_str(), in);
    out_dir = buf;
    sprintf(buf, "%d%s", in, ".cov");
    cov_file = cov_dir + buf;
    sprintf(buf, "%d%s", in, ".time");
    time_file = time_dir + buf;
    
    cout << "out_dir: " << out_dir << endl;
    cout << cov_file << endl;
    cout << time_file << endl;
    getchar();
    
    TimeRecord initialize_data_rec(out_dir + initialize_data_file);
    TimeRecord initialize_cuda_rec(out_dir + "initialize_cuda_file");
    TimeRecord front0_rec(out_dir + "front0_file");
    
    initialize_data_rec.Start();

#ifdef CUDA_CROSS
    if (CudaInitData(initialize_cuda_rec) == 0) {
#elif defined CUDA_FITNESS
    if (CudaInitData(initialize_cuda_rec) == 0) {
#else
    if (InitData() != 0) {
#endif
      cerr << "initialize failed" << endl;
    } else {
      cout << "initialize succeeded" << endl;
    }
    initialize_data_rec.Record();
    
    for (int i = 0; i < ite_num; ++i) {
      cur_ite_num = i;
      char buf[128];
      sprintf(buf, "%d//", cur_ite_num);
      cur_gen_dir = buf;
      system(("mkdir " + out_dir + cur_gen_dir).c_str());
      
      TimeRecord initialize_pop_rec(out_dir + cur_gen_dir + initialize_pop_file);
      //    TimeRecord initialize_fit_rec(out_dir + cur_gen_dir + initialize_pop_file);
      TimeRecord first_comp_fit_rec(out_dir + cur_gen_dir + first_com_fit_file);
      TimeRecord father_select_rec(out_dir + cur_gen_dir + fatherselect_file);
      TimeRecord crossover_rec(out_dir + cur_gen_dir + crossover_file);
      TimeRecord mutation_rec(out_dir + cur_gen_dir + mutation_file);
      TimeRecord elite_rec(out_dir + cur_gen_dir + eliteselect_file);
      TimeRecord comp_fit_rec(out_dir + cur_gen_dir + computefitness_file);
      TimeRecord cuda_cross_sel_rec(out_dir + cur_gen_dir + "cuda_crossselect");
      
      CudaTimeRecord cuda_crossover_in_rec(out_dir + cur_gen_dir + "cuda_crossover_in");
      CudaTimeRecord cuda_crossover_rec(out_dir + cur_gen_dir + cuda_crossover_file);
      CudaTimeRecord cuda_crossover_out_rec(out_dir + cur_gen_dir + "cuda_crossover_out");

      CudaTimeRecord cuda_comp_fit_in_rec(out_dir + cur_gen_dir + "cuda_computefitness_in");
      CudaTimeRecord cuda_comp_fit_rec(out_dir + cur_gen_dir + cuda_computefitness_file);
      CudaTimeRecord cuda_comp_fit_out_rec(out_dir + cur_gen_dir + "cuda_computefitness_out");

      CudaTimeRecord cuda_init_fit_rec(out_dir + cur_gen_dir + cuda_init_fitness_file);
      
      cur_ite_num = -1;
      cout << "initpop()" << endl;
      initialize_pop_rec.Start();
      InitPop();

#ifdef CUDA_FITNESS
      CudaCompFit(cuda_init_fit_rec, pop_num);
#else
      CompFit(0, pop_num);
#endif
      initialize_pop_rec.Record();

      /*      
      vector<double> t_apsc;
      vector<double> t_time;
      vector<int> t_total;
      vector<int> t_first;
      vector<int> t_list;
      
      t_apsc.resize(pop_num << 1);
      t_time.resize(pop_num << 1);
      t_total.resize(pop_num << 1);
      t_first.resize(pop_num * code_num * 2);
      t_list.resize(pop_num * 3);
      
      for (int q = 0; q < pop_num; ++q) {
        for (int k = 0; k < code_num; ++k) {
          t_first[q * code_num + k] = pop_vec[q].first_exe_vec[k];
        }
      }
      
      first_comp_fit_rec.Start();
      CompFit(0, pop_num);
      first_comp_fit_rec.Record();
      
      for (int q = 0; q < pop_num; ++q) {
        for (int k = 0; k < code_num; ++k) {
          if (t_first[q * code_num + k] != pop_vec[q].first_exe_vec[k]) {
            cout << "Init Wrong" << endl;
            cout << q << " " << k << endl;
            cout << t_first[q * code_num + k] << " " << pop_vec[q].first_exe_vec[k] << endl;
          }
        }
      }
      cout << "Init Right!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
      */

      cur_ite_num = i;
      
      for (int j = 0; j < gen_num; ++j) {
        cur_gen_num = j;
        cout << "j: " << j << endl;
        cout << "FatherSelect();" << endl;
        father_select_rec.Start();
        FatherSelect(pop_num);
        father_select_rec.Record();
        
        cout << "CrossOver();" << endl;
        
        
        //      SinglePointCrossover();
#ifdef CUDA_CROSS
        CudaScanCross(cuda_cross_sel_rec, cuda_crossover_rec, cuda_crossover_in_rec, cuda_crossover_out_rec);
#else
        crossover_rec.Start();
        OrderCrossOver();
        //        ScanCrossOver();
        crossover_rec.Record();
#endif
        
        cout << "Mutation();" << endl;
        mutation_rec.Start();
        Mutation(pop_num, pop_num);
        mutation_rec.Record();
        
        cout << "CompFit();" << endl;

#ifdef CUDA_FITNESS
        CudaCompFit2(cuda_comp_fit_rec, cuda_comp_fit_in_rec, cuda_comp_fit_out_rec, pop_num);
#else
        comp_fit_rec.Start();
        CompFit(pop_num, pop_num);
        comp_fit_rec.Record();
#endif


        /*          
        cout << "Save" << endl;
        cout << pop_num << endl;
        for (int k = pop_num; k < (pop_num << 1); ++k) {
          t_apsc[k] = pop_vec[k].apsc;
          t_time[k] = pop_vec[k].eff_time;
        }
          
        for (int q = 0; q < pop_num * 2; ++q) {
          for (int k = 0; k < code_num; ++k) {
            t_first[q * code_num + k] = pop_vec[q].first_exe_vec[k];
          }
        }
        
        CudaCompFit2(cuda_comp_fit_rec, pop_num);
        
        for (int q = 0; q < pop_num * 2; ++q) {
          for (int k = 0; k < code_num; ++k) {
            if (t_first[q * code_num + k] != pop_vec[q].first_exe_vec[k]) {
              cout << "Compt Wrong" << endl;
                cout << q << " " << k << endl;
                cout << t_first[q * code_num + k] << " " << pop_vec[q].first_exe_vec[k] << endl;
                getchar();
            }
          }
        }
        cout << "Comp Right!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
        cout << "Count" << endl;
        */

        elite_rec.Start();
        NonDomSort(0, pop_num << 1);
        ElismSelect();
        elite_rec.Record();
        
        cout << "ElismSelect()" << endl;
      }

      cout << "End for" << endl;
      LogFront0(front0_rec);
      front0_rec.Comment("\n");
    }
#ifdef CUDA_CROSS
    CudaDeleteData();
#elif defined CUDA_FITNESS
    CudaDeleteData();
#endif

  }
  cout << "Real End" << endl;
  return 0;
}
  
