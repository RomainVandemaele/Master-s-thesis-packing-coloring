#include "Cplex.hpp"

//Ex from pdf
void Cplex::test() {
  IloModel model(env);
  IloNumVarArray var(env);
  IloRangeArray con(env);
  var.add(IloNumVar(env, 0, IloInfinity,ILOFLOAT));
  var.add(IloNumVar(env, 0, IloInfinity,ILOFLOAT));
  var.add(IloNumVar(env, 0, IloInfinity,ILOFLOAT));
  var.add(IloNumVar(env, 0, IloInfinity,ILOFLOAT));
  IloObjective obj = IloMinimize(env , var[0]+var[1]+var[2]+var[3]);
  model.add(obj);

  IloRange ctr1 ( env, 44 , var[0] ,IloInfinity );
  IloRange ctr2 ( env, 3 , var[1] , IloInfinity);
  IloRange ctr3 ( env, 48 , var[2] + 3*var[3] , IloInfinity);

  con.add(ctr1);
  con.add(ctr2);
  con.add(ctr3);

  model.add(con);


  IloCplex cplex(model);

  if ( !cplex.solve() ) {
     env.error() << "Failed to optimize LP" << endl;
     throw(-1);
  }

  IloNumArray vals(env);
  cplex.getDuals(vals, con);
  std::cout << "Duals : " << vals << '\n';
  env.end();
}


void Cplex::test2() { //Dual
  IloEnv env2;
  IloModel model(env2);
  IloNumVarArray var(env2);
  IloRangeArray con(env2);
  for (size_t i = 0; i < N; i++) { //pi_v
    var.add(IloNumVar(env2, 0, 1,ILOFLOAT));
  }
  for (size_t j = 0; j < D-1; j++) {
    var.add(IloNumVar(env2, 0, 1,ILOFLOAT));
  }
  IloExpr obj = var[0];
  for (size_t j = 1; j < N  ; j++) {
    obj += var[j];
  }
  for (size_t k = 1; k < D  ; k++) {
    const int c = k;
    obj += var[N-1 + k]*c;
  }
  std::cout << "TEST 2" << '\n';
  model.add(IloMaximize(env2, obj));
  obj.end();

  for (size_t i = 0; i < I; i++) {
    IloExpr exprConstraint(env2);
    for (size_t j = 0; j < N; j++) {
      if(incidence[i][j]) {
        exprConstraint += var[j];
      }
    }
    for (size_t k = 1; k < D  ; k++) {
      if(classification[i] >= k+1) {
        exprConstraint += var[N+k -1];
      }
    }
    IloRange ctr1 ( env2, 0 , exprConstraint , 1);
    exprConstraint.end();
    con.add(ctr1);
  }

  //must be one


  model.add(con);


  IloCplex cplex(model);

  if ( !cplex.solve() ) {
     env2.error() << "Failed to optimize LP" << endl;
     throw(-1);
  }
  env2.out() << "Solution status = " << cplex.getStatus() << endl;
  env2.out() << "Solution value  = " << cplex.getObjValue() << endl;
  IloNumArray vals(env2);
  cplex.getDuals(vals, con);
  std::cout << "Duals : " << vals << '\n';
  std::cout << "RESULT : " << '\n';
  for (size_t i = 0; i < N+D-1; i++) {
    if(cplex.getValue(var[i]) == 1) {
      std::cout << "1 ";
    }else {
      std::cout << cplex.getValue(var[i]) << " ";
    }
  }
  env2.end();
}

Cplex::Cplex() {}

Cplex::Cplex(const char* filename, int mode) : mod(mode) {
    if(mod == VERTEX_COLOR || mod == VERTEX_COLOR_2) {
      importFileColor(filename);
    }else if(mod == INDEPENDENT ) {
      importFileStable(filename,false);
    }else if(mod == BRANCH_AND_PRICE) {
      importFileStable(filename,true);
    }
}

void Cplex::importFileColor(const char* filename) {
  FILE * file;
  file = fopen( filename, "r");
  myFscanf(file, "%u", &N);
  myFscanf(file, "%u", &D);
  distances = new unsigned int*[N];
  for (size_t i = 0; i < N; i++) {
    distances[i] = new unsigned int[N];
    for (size_t j = 0; j < N; j++) {
      myFscanf(file, "%u", &distances[i][j]);
    }
  }

  fclose(file);
}

void Cplex::importFileStable(const char* filename,bool dist) {

  FILE * file;
  file = fopen( filename, "r");
  myFscanf(file, "%u", &N);
  myFscanf(file, "%u", &D);
  myFscanf(file, "%u", &I);
  std::cout << N << "  " << D << " "  << I << std::endl;
  for (size_t i = 0; i < I; i++) {
    incidence.push_back(new bool[N]);
    unsigned int cat = 0;
    myFscanf(file, "%u", &cat);
    classification.push_back(cat);
  }

  unsigned char c;
  myFscanf(file, "%c", &c);
  myFscanf(file, "%c", &c);
  int s = (int) ceil(N/8.0);
  std::cout << "S : " << s << std::endl;
  for (size_t j = 0; j < I; j++) {
    short unsigned int mask = 128;
    unsigned int counter = 0;
    for (int k = 0; k < s; k++) {
      myFscanf(file, "%c", &c);
      for (size_t l = 0; l < 8; l++) {
        if(counter < N) {
          incidence[j][counter] = ( c & mask ) > 0;
        }
        mask = mask >> 1;
        counter++;
      }
      mask = 128;
    }
    myFscanf(file, "%c", &c); //read '\n'
  }

  if(dist) {
    distances = new unsigned int*[N];
    for (size_t i = 0; i < N; i++) {
      distances[i] = new unsigned int[N];
      for (size_t j = 0; j < N; j++) {
        myFscanf(file, "%u", &distances[i][j]);
      }
    }
    std::cout << "distance 0 1" << distances[0][1] << '\n';
  }

  fclose(file);
}

void Cplex::modelise() {
  try {
     IloModel model(env);
     IloNumVarArray var(env);
     IloRangeArray con(env);
     if(mod == VERTEX_COLOR) {
       this->createModelColor(model,var,con);
     }else if(mod == VERTEX_COLOR_2) {
       this->createModelColor2(model,var,con);
     }else if(mod == INDEPENDENT) {
       this->createModelStable(model,var,con);
     }
     if(mod != BRANCH_AND_PRICE) {
       IloCplex cplex(model);
       this->solveModel(cplex,var,con);
       env.end();
     }else if(mod == BRANCH_AND_PRICE) {
       int i=0;
       std::cout << "OK : " << i << '\n';
       while (i < 5) { //while(1 - cplexS.getObjValue() < 0 )
         IloEnv envP;
         IloModel model(envP);
         IloNumVarArray var(envP);
         IloRangeArray con(envP);
         this->createModelStable(model,var,con);

         IloCplex cplexP(model);
         std::cout << "\nPRIMAL : \n" << '\n';
         IloNumArray duals = this->solveModel(cplexP,var,con);
         envP.end();

         IloEnv envS;
         IloModel modelSub(envS);
         IloNumVarArray varSub(envS);
         IloRangeArray conSub(envS);
         std::cout << "\nSUBPROBLEM : \n" << '\n';
         this->createModelSubProblem(modelSub,varSub,conSub,duals);
         IloCplex cplexS(modelSub);
         bool* newSubset = solveSubProblem(cplexS,varSub,conSub);
         addColumn(newSubset); //Adapt primal : adding new subset
         envS.end();
         //test2();
         ++i;
       }


     }
  }
  catch (IloException& e) {
     cerr << "Concert exception caught: " << e << endl;
  }
  catch (...) {
     cerr << "Unknown exception caught" << endl;
  }


}

void Cplex::addColumn(bool* column) {
  I++;
  incidence.push_back(new bool[N]);
  for (size_t i = 0; i < N; i++) {
    incidence[I-1][i] = column[i];
    //std::cout << "OK 1.45 : " << i << std::endl;
  }
  //classify stable
  unsigned int min =  INF;
  for (size_t i = 0; i < N; i++) {
    for (size_t j = i+1; j < N; j++) {
      if(column[i] && column[j]) {
          if(distances[i][j] < min) {
            min = distances[i][j];
          }
      }
    }
  }
  std::cout << "NEW COLUMN CATEGORY : " << min << '\n';
  classification.push_back(min);
  delete[] column;
}

void Cplex::createModelColor(IloModel model, IloNumVarArray x, IloRangeArray c) {
  IloEnv env = model.getEnv();
  for (size_t i = 0; i < N; i++) {
    for (size_t j = 0; j < N; j++) {
      x.add(IloNumVar(env, 0, 1,ILOBOOL));
    }
  }
  //x_ij = x[i*N + j]
  x.add(IloNumVar(env, 0, IloInfinity ,ILOINT)); //z

  //min z the number of color
  IloExpr obj = x[N*N];
  model.add(IloMinimize(env, obj));
  obj.end();

  for (size_t i = 0; i < N; i++) { //for each vertex
    IloExpr exprConstraint(env);
    for (size_t j = 0; j < N; j++) {
        exprConstraint += x[i*N + j]; //sum of all color
    }
    //must be one
    IloRange ctr1 ( env, 1 , exprConstraint , 1);
    model.add(ctr1);
    exprConstraint.end();
  }

  //for each pair of nodes i,j
  for (size_t i = 0; i < N; i++) {
    for (size_t j = i+1; j < N; j++) {
      //cannot have same color k if k < d(i,j)
        for (size_t k = 0; k < N; k++) {
          if(distances[i][j] <= k +1 ) {
            IloExpr exprConstraint(env);
            //std::cout << i << " , " << j << " : " << k << std::endl;
            exprConstraint += x[i*N + k] + x[j*N + k]; //sum of all color
            IloRange ctr2 ( env, 0 , exprConstraint , 1);
            model.add(ctr2);
            exprConstraint.end();
          }
        }

    }
  }

  for (size_t i = 0; i < N; i++) { //color i
    for (size_t v = 0; v < N; v++) { //vertex v
      IloExpr exprConstraint(env);
      exprConstraint += ( (int) (i+1) ) * x[v*N + i] - x[N*N] ; //sum of all color
      IloRange ctr3 ( env, -IloInfinity , exprConstraint , 0);
      model.add(ctr3);
      exprConstraint.end();
    }
  }
}

void Cplex::createModelColor2(IloModel model, IloNumVarArray x, IloRangeArray c) {
  IloEnv env = model.getEnv();
  for (size_t i = 0; i < N; i++) {
    for (size_t j = 0; j < D; j++) {
      x.add(IloNumVar(env, 0.0, 1.0,ILOBOOL));
    }
  }

  //x_ij = x[i*N + j]

  IloExpr obj(env);
  for (size_t i = 0; i < N; i++) {
    for (size_t k = 0; k < D-1; k++) {
      obj -= x[i*D + k];
    }
  }
  obj +=  ((int) (D-1)) + (int) N;
  model.add(IloMinimize(env, obj));
  obj.end();

  for (size_t i = 0; i < N; i++) { //for each vertex
    IloExpr exprConstraint(env);
    for (size_t j = 0; j < D; j++) {
        exprConstraint += x[i*D + j]; //sum of all color
    }
    //must be one
    IloRange ctr1 ( env, 1 , exprConstraint , 1);
    model.add(ctr1);
    exprConstraint.end();
  }

  //for each pair of nodes i,j
  for (size_t i = 0; i < N; i++) {
    for (size_t j = i+1; j < N; j++) {
      //cannot have same color k if k < d(i,j)
        for (size_t k = 0; k < D-1; k++) {
          if(distances[i][j] <= k +1 ) {
            IloExpr exprConstraint(env);
            //std::cout << i << " , " << j << " : " << k << std::endl;
            exprConstraint += x[i*D + k] + x[j*D + k]; //sum of all color
            IloRange ctr2 ( env, 0 , exprConstraint , 1);
            model.add(ctr2);
            exprConstraint.end();
          }
        }

    }
  }
}

void Cplex::createModelStable(IloModel model, IloNumVarArray x, IloRangeArray c) {

  IloEnv env = model.getEnv();
  for (size_t i = 0; i < I; i++) {
    if(mod == BRANCH_AND_PRICE) {
      x.add(IloNumVar(env, 0.0, 1.0,ILOFLOAT));
    }else {
      x.add(IloNumVar(env, 0.0, 1.0,ILOBOOL));
    }

  }

  IloExpr obj = x[0];
  for (size_t j = 1; j < I; j++) {
    obj += x[j];
  }
  model.add(IloMinimize(env, obj));
  obj.end();

  for (size_t i = 0; i < N; i++) {
    IloExpr exprConstraint(env);
    for (size_t j = 0; j < I; j++) {
      if(incidence[j][i]) {
        exprConstraint += x[j];
      }
    }
    IloRange ctr1 ( env, 1 , exprConstraint , 1);
    c.add(ctr1);
    //model.add(ctr1);
    exprConstraint.end();
  }

  for (size_t i = 1; i < D; i++) {
    IloExpr exprConstraint(env);
    for (size_t j = 0; j < I; j++) {
      if(classification[j] <= i +1) {
        exprConstraint += x[j];
      }
    }
    IloRange ctr2 ( env, 0 , exprConstraint , i );
    c.add(ctr2);
    exprConstraint.end();
  }

  model.add(c);

}


void Cplex::createModelSubProblem(IloModel model, IloNumVarArray x, IloRangeArray c,IloNumArray dual) {

  IloEnv env = model.getEnv();
  for (size_t i = 0; i < N; i++) { //Yi : indicates if nodes i is in the new column(stable set)
    x.add(IloNumVar(env, 0.0, 1.0,ILOBOOL));
  }

  for (size_t a = 0; a < D-1; a++) { //alpha j : indicates if set is of category j<=
    x.add(IloNumVar(env, 0.0, 1.0,ILOBOOL));
  }

  for (size_t k = 0; k < D-1; k++) {
    for (size_t i = 0; i < N; i++) { //Zijk : indicates if nodes i and j are taken together and ak = 1
      for (size_t j = 0; j < N; j++) {
        x.add(IloNumVar(env, 0.0, 1.0,ILOBOOL));
      }
    }
  }

  //env.out() << "Duals = " << dual << endl;
  IloExpr obj = 1 - x[0] * dual[0];
  for (size_t j = 1; j < N + (D-1) ; j++) {
    obj -= (x[j] * dual[j]);
  }
  model.add(IloMinimize(env, obj));
  obj.end();

  IloExpr exprConstraint(env);
  exprConstraint += x[N]; //alpha1 = 1 => force stable set
  IloRange ctr0 ( env, 1 , exprConstraint , 1);
  c.add(ctr0);
  exprConstraint.end();

  //std::cout << "OOOKKK 1" << '\n';
  for (size_t i = 0; i < D -2; i++) { //alpha j <= alpha j-1 =>  0 <= alpha j-1 - alpha j <= 1
    IloExpr exprConstraint(env);
    exprConstraint += x[N + i ]; //index N = alpha_1
    exprConstraint -= x[N + i + 1];
    IloRange ctr1 ( env, 0 , exprConstraint , 1);
    c.add(ctr1);
    exprConstraint.end();
  }
  //std::cout << "OOOKKK 2" << '\n';
  for (size_t i = 0; i < N; i++) { //Zijk = Zjik
    for (size_t j = i+1; j < N; j++) {
      for (size_t k = 0; k < D-1; k++) {
        IloExpr exprConstraint(env);
        exprConstraint +=  x[N + (D-1) + k*N*N + i*N + j];
        exprConstraint -=  x[N + (D-1) + k*N*N + j*N + i];
        IloRange ctr2 ( env, 0 , exprConstraint , 0);
        c.add(ctr2);
        exprConstraint.end();
      }
    }
  }
  //std::cout << "OOOKKK 3" << '\n';

  for (size_t i = 0; i < N; i++) {
    for (size_t j = i+1; j < N; j++) {
      for (size_t k = 1; k < D; k++) { // z_ijk* (k+1) <= Dij * Zijk
        IloExpr exprConstraint(env);
        const int d = distances[i][j];
        exprConstraint +=  x[N + (D-1) + (k-1)*N*N + i*N + j] * d;
        const int d2 = k+1;
        exprConstraint -=  x[N + (D-1) + (k-1)*N*N + i*N + j] * d2;
        IloRange ctr3 ( env, 0 , exprConstraint , D);
        c.add(ctr3);
        exprConstraint.end();
      }
    }
  }
  //std::cout << "OOOKKK 4" << '\n';

  //constraint to define zijk : zijk <= Yi/Yj/ak and a_k + Yi + Yj - 2 <= Zijk
  for (size_t i = 0; i < N; i++) {
    for (size_t j = i+1; j < N; j++) { // Zijk* (k+1) <= Dij * Zijk
      for (size_t k = 0; k < D-1; k++) {
        if(i!=j ) {
          IloExpr exprConstraint1(env);
          //IloExpr exprConstraint2(env);
          IloExpr exprConstraint3(env);
          IloExpr exprConstraint4(env);
          exprConstraint1 +=  x[i] ;
          exprConstraint1 -=  x[N + (D-1) + k*N*N + i*N + j];
          //exprConstraint2 +=  x[j] ;
          //exprConstraint2 -=  x[N + (D-1) + k*N*N + i*N + j];
          exprConstraint3 +=  x[N + k] ;
          exprConstraint3 -=  x[N + (D-1) + k*N*N + i*N + j];
          exprConstraint4 +=  x[N + (D-1) + k*N*N + i*N + j];
          exprConstraint4 -=  (x[N + k] + x[i] + x[j] -2);

          IloRange ctr3 ( env, 0 , exprConstraint1 , 1);
          //IloRange ctr4 ( env, 0 , exprConstraint2 , 1);
          IloRange ctr5 ( env, 0 , exprConstraint3 , 1);
          IloRange ctr6 ( env, 0 , exprConstraint4 , 2);

          c.add(ctr3);
          //c.add(ctr4);
          c.add(ctr5);
          c.add(ctr6);
          exprConstraint1.end();
          //exprConstraint2.end();
          exprConstraint3.end();
          exprConstraint4.end();
        }
      }
    }
  }
  //std::cout << "OOOKKK 5" << '\n';
  model.add(c);
}

bool* Cplex::solveSubProblem(IloCplex cplex,IloNumVarArray var, IloRangeArray con) {
  //std::cout << "OOOKKK 6" << '\n';
  if ( !cplex.solve() ) {
     env.error() << "Failed to optimize LP" << endl;
     throw(-1);
  }
  //std::cout << "OOOKKK 7" << '\n';
  bool* subset = new bool[N];
  int k=0;
  for (size_t i = 0; i < N; i++) {
    subset[i] = false;
    if(cplex.getValue(var[i]) == true) { //node i in stable set
      std::cout << "VERTEX " << i << " is in the new column" << '\n';
      k++;
      subset[i] = true;
    }
  }

  for (size_t i = 0; i < D - 1 ; i++) {
    if(cplex.getValue(var[N+i]) == true) { //node i in stable set
      std::cout << "1 ";
    }else {
      std::cout << "0 ";
    }
  }
  std::cout << "\n";

  env.out() << "Solution status = " << cplex.getStatus() << endl;
  env.out() << "Solution value  = " << cplex.getObjValue() << endl;
  env.out() << "Solution size = "   << k << endl;
  return subset;
}

IloNumArray Cplex::solveModel(IloCplex cplex,IloNumVarArray var, IloRangeArray con) {
  // Optimize the problem and obtain solution.
  if ( !cplex.solve() ) {
     env.error() << "Failed to optimize LP" << endl;
     throw(-1);
  }

  if(mod == INDEPENDENT || mod == BRANCH_AND_PRICE) {
    for (size_t i = 0; i < I; i++) {
      if(cplex.getValue(var[i]) > 0) {
        std::cout << "Stable set " << i << " of category "<< classification[i] << " was choosen (" << cplex.getValue(var[i]) << ")" << std::endl;
        if(classification[i]  == INF) {
          for (size_t v = 0; v < N; v++) {
            if(incidence[i][v]) {
              std::cout << "\tcomposed of vertex " << v << '\n';
            }
          }
        }
      }
    }
  }else if (mod == VERTEX_COLOR) {
    for (size_t i = 0; i < N; i++) {
      for (size_t j = 0; j < N; j++) {
        if(cplex.getValue(var[i*N + j]) == true) {
          std::cout << "Vertex " << i << " has color "<< j+1 << std::endl;
        }
      }
    }
  }else if (mod == VERTEX_COLOR_2) {
    for (size_t i = 0; i < N; i++) {
      for (size_t j = 0; j < D; j++) {
        if(cplex.getValue(var[i*D + j]) == true) {
          std::cout << "Vertex " << i << " has color "<< j+1 << std::endl;
        }
      }
    }
  }



  //cplex.getValues(vals, var);
  //env.out() << "Values        = " << vals << endl;

  env.out() << "Solution status = " << cplex.getStatus() << endl;
  env.out() << "Solution value  = " << cplex.getObjValue() << endl;
  //cplex.getSlacks(vals, con);
  //env.out() << "Slacks        = " << vals << endl;


  IloNumArray vals(env);
  cplex.getDuals(vals, con);
  if(mod == BRANCH_AND_PRICE) {
    env.out() << "Duals = " << vals << endl;
    IloNumArray cost(env);
    cplex.getReducedCosts(cost, var);
    env.out() << "Reduced Costs = " << cost << endl;
    return vals;
  }

  //cplex.writeSolution("sol.txt");
  //cplex.exportModel("lpex1.lp");
}
