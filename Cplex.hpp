#ifndef CPLEX_HPP
#define CPLEX_HPP

#define VERTEX_COLOR        1
#define VERTEX_COLOR_2      2
#define INDEPENDENT         3
#define BRANCH_AND_PRICE    4
#define INF 10000

#include "common.hpp"

#include <iostream>
#include <fstream>
#include <string.h>
#include <math.h>
#include <vector>

#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

class Cplex {
  private :
    unsigned int N;
    unsigned int D;
    unsigned int I;
    unsigned int** distances;

    std::vector<unsigned int> classification;
    std::vector<bool*> incidence;

    int mod;
    IloEnv env;

  public :

  Cplex();
  Cplex(const char* filename, int mode);
  void importFileColor(const char* filename);
  void importFileStable(const char* filename,bool dist);

  void modelise();
  void createModelColor(IloModel model, IloNumVarArray x, IloRangeArray c);
  void createModelColor2(IloModel model, IloNumVarArray x, IloRangeArray c);
  void createModelStable(IloModel model, IloNumVarArray x, IloRangeArray c);
  float* solveModel(IloCplex cplex,IloNumVarArray x, IloRangeArray c);

  void createModelSubProblem(IloModel model, IloNumVarArray x, IloRangeArray c,float* dual);
  bool* solveSubProblem(IloCplex cplex,IloNumVarArray var, IloRangeArray con);
  void addColumn(bool* vertices);

  void test();
  float* test2();


};

#endif
