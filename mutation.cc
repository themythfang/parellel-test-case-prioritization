#include "mutation.h"
#include "gene.h"
#include "initdata.h"
#include "cuda_initdata.h"
#include <time.h>
#include <stdlib.h>

// Randomly swap two gene.
// @lista, @listb, the swap gene position of all the individuals.
void Mutation(int pos, int num) {
  srand(time(NULL));
  // Randomly choose two position of the individual.
  //  for (int i = pos; i < (pos + pop_num << 1); ++i) {
  for (int i = pos; i < (pos + num); ++i) { 
    int pos_a = rand() % test_num;
    int pos_b = rand() % test_num;

    while(pos_b == pos_a) {
      pos_b = rand() % test_num;
    }
    if (pos_a > pos_b) {
      swap(pos_a, pos_b);
    }
    swap_vec_a[i] = pos_a;
    swap_vec_b[i] = pos_b;

    swap(pop_vec[i].gene_code_vec[pos_a], pop_vec[i].gene_code_vec[pos_b]);
  }
}
