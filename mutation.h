#ifndef MUTATION_H_
#define MUTATION_H_

#include <vector>
using namespace std;

extern vector<int> lista;
extern vector<int> listb;

// Randomly swap two gene.
// @lista, @listb, the swap gene position of all the individuals.
void Mutation(int pos, int num);

#endif //  MUTATION_H_
