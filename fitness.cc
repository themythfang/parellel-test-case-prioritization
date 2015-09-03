#include "gene.h"
#include "initdata.h"

void CompIndiFit(Individual& indi) {
  //  int gen_code;
  // The minimize test number (gene number) that needs to lead to
  // max code coverage.
  int min_exe_num = MIN_NUM;
  // The total statement that is sufficient.
  int total_sta = 0;
  // The effective execution time.
  double eff_time = 0;

  int first = 0;
  for (int i = 0; i < code_num; ++i) {
    if (indi.first_exe_vec[i] != NOEXIST) {
      int j;
      for (j = 0; j < test_num; ++j) {
	int gen_code = indi.gene_code_vec[j];
	if (gen_vec[gen_code].coverage[i] == '1') {
	  indi.first_exe_vec[i] = j;
	  min_exe_num = max(min_exe_num, j);
	  total_sta += (j + 1);
	  break;
	}
      }
      if (j == test_num) {
	indi.first_exe_vec[i] = NOEXIST;
      }
    }
  }

  for (int i = 0; i <= min_exe_num; ++i) {
    int gen_code = indi.gene_code_vec[i];
    eff_time += gen_vec[gen_code].time;
  }

  indi.total_sta = total_sta;
  indi.apsc = (1.0 - total_sta * 1.0 / (code_num * test_num) + 1.0 / (test_num << 1));
  //  indi.apsc = total_sta;
  indi.eff_time = eff_time;
  //  indi.eff_time = indi.gene_code_vec[50];
  //  indi.eff_time = min_exe_num;
  //  indi.eff_time = tt;
}

void CroUpdIndiFit(Individual& f_indi, Individual& indi) {
  int gen_code;
  vector<int> t_vec;
  t_vec.resize(code_num);
  int cross_num = cross_propo * test_num;

  for (int i = 0; i < code_num; ++i) {
    if (f_indi.first_exe_vec[i] == NOEXIST) {
      t_vec[i] = NOEXIST;
    } else {
      t_vec[i] = indi.pos_vec[f_indi.first_exe_vec[i]];
    }
  }

  for (int i = 0; i < code_num; ++i) {
    // Only handle the uncrossovered elements.
    if (t_vec[i] != NOEXIST && t_vec[i] >= cross_num) {
      for (int j = indi.flags_vec[f_indi.first_exe_vec[i]]; j < cross_num; ++j) {
        gen_code = indi.gene_code_vec[j];
        if (gen_vec[gen_code].coverage[i] == '1') {
          t_vec[i] = j;
          break;
        }
      }
    }
  }

  int total_sta = 0;
  double eff_time = 0;
  int min_gen_num = MIN_NUM;

  for (int i = 0; i < code_num; ++i) {
    if (t_vec[i] != NOEXIST) {
      total_sta += (t_vec[i] + 1);
      min_gen_num = max(min_gen_num, t_vec[i]);
    }
  }
  for (int i = 0; i <= min_gen_num; ++i) {
    gen_code = indi.gene_code_vec[i];
    eff_time += gen_vec[gen_code].time;
  }

  for (int i = 0; i < code_num; ++i) {
    indi.first_exe_vec[i] = t_vec[i];
  }

  indi.total_sta = total_sta;
  indi.apsc = (1.0 - total_sta * 1.0 / (code_num * test_num) + 1.0 / (test_num << 1));
  indi.eff_time = eff_time;
}

void MutaUpdIndiFit(Individual& indi, int a, int b) {
  int min_ab = a;
  int max_ab = b;

  int gen_code;
  vector<int> t_vec;
  t_vec.resize(code_num);

  for (int i = 0; i < code_num; ++i) {
    t_vec[i] = indi.first_exe_vec[i];
  }

  for (int i = 0; i < code_num; ++i) {
    if (t_vec[i] != NOEXIST) {
      if (max_ab > t_vec[i]) {
	if (min_ab == t_vec[i]) {
	  // t_vec[i] = min_ab < max_ab 
	  for (int j = min_ab; j <= max_ab; ++j) {
	    int gen_code = indi.gene_code_vec[j];
	    if (gen_vec[gen_code].coverage[i] == '1') {
	      t_vec[i] = j;
	      break;
	    }
	  }
	} else if (min_ab < t_vec[i]) {
	  // min_ab < t_vec[i] < max_ab
	  int gen_code = indi.gene_code_vec[min_ab];
	  if (gen_vec[gen_code].coverage[i] == '1') {
	    t_vec[i] = min_ab;
	  } 
	}
	// max_ab > min_ab > t_vec[i] : no influence to t_vec[i]
      } else if (max_ab == t_vec[i]) {
	// min_ab < max_ab = t_vec[i]
	t_vec[i] = min_ab;
      }
      // min_ab < max_ab < t_vec[i] : no influence to t_vec[i]
    }
  }

  int total_sta = 0;
  double eff_time = 0;
  int min_gen_num = MIN_NUM;
  for (int i = 0; i < code_num; ++i) {
    if (t_vec[i] != NOEXIST) {
      total_sta += (t_vec[i] + 1);
      min_gen_num = max(min_gen_num, t_vec[i]);
    }
  }
  for (int i = 0; i <= min_gen_num; ++i) {
    gen_code = indi.gene_code_vec[i];
    eff_time += gen_vec[gen_code].time;
  }

  for (int i = 0; i < code_num; ++i) {
    indi.first_exe_vec[i] = t_vec[i];
  }

  indi.total_sta = total_sta;
  indi.apsc = (1.0 - total_sta * 1.0 / (code_num * test_num) + 1.0 / (test_num << 1));
  indi.eff_time = eff_time;
}

void UpSon(int p1, int p2, int s, int k) {
  // Update algorithm.
  for (int j = 0; j < code_num; ++j) {
    int first_exe_code = pop_vec[p1].first_exe_vec[j];
    pop_vec[s].first_exe_vec[j] = first_exe_code;
    if (first_exe_code != NOEXIST) {
      if (first_exe_code < k) {
	pop_vec[s].first_exe_vec[j] = first_exe_code;
      } else {
	first_exe_code = pop_vec[p2].first_exe_vec[j];
	pop_vec[s].first_exe_vec[j] = pop_vec[s].single_pos_vec[first_exe_code];
      }
    } else {
      pop_vec[s].first_exe_vec[j] = NOEXIST;
    }
  }
}

void GetSonFit(int code) {
  int min_exe_num = MIN_NUM;
  int total_sta = 0;
  double eff_time = 0;
  for (int j = 0; j < code_num; ++j) {
    int t = pop_vec[code].first_exe_vec[j];
    min_exe_num = max(min_exe_num, t);
    total_sta += (t + 1);
  }

  for (int j = 0; j <= min_exe_num; ++j) {
    int gen_code = pop_vec[code].gene_code_vec[j];
    eff_time += gen_vec[gen_code].time;
  }
  pop_vec[code].total_sta = total_sta;
  pop_vec[code].apsc = (1.0 - total_sta * 1.0 / (code_num * test_num) + 1.0 / (test_num << 1));
  pop_vec[code].eff_time = eff_time;
}

// Get the coverage proportion of the individual.
void CompFit(int pos, int num) {
  for (int i = pos; i < pos + num; ++i) {
    CompIndiFit(pop_vec[i]);
  }
}

void CroUpdFit(int pos, int num) {
  for (int i = pos; i < pos + num; ++i) {
    CroUpdIndiFit(pop_vec[pop_vec[i].father], pop_vec[i]);
  }
}

void MutaUpdFit(int pos, int num) {
  for (int i = pos; i < pos + num; ++i) {
    MutaUpdIndiFit(pop_vec[i], swap_vec_a[i], swap_vec_b[i]);
  }
}

void SinCroUpdFit(int pos, int num) {
  for (int i = pos; i < pos + num; ++i) {
    int p1 = pop_vec[i].father;
    int p2 = pop_vec[i].mother;

    UpSon(p1, p2, i, single_rand_vec[(i - pos) / 2]);
    GetSonFit(i);
  }
}
/*
void Output(int num) {
  cout << "father: " << pop_vec[num].father << " " << pop_vec[num].mother << endl;
  cout << "K: " << (num - pos) / 2 << " " << single_rand_vec[(num - pos) / 2] << endl;

  for (int j = 0; j < code_num; ++j) {
    int first_exe_code = pop_vec[p1].first_exe_vec[j];
    if (first_exe_code != NOEXIST) {
      if (first_exe_code < k) {
	pop_vec[s].first_exe_vec[j] = first_exe_code;
      } else {
	first_exe_code = pop_vec[p2].first_exe_vec[j];
	pop_vec[s].first_exe_vec[j] = pop_vec[s].single_pos_vec[first_exe_code];
      }
    } else {
      pop_vec[s].first_exe_vec[j] = NOEXIST;
    }
  }
}
*/
