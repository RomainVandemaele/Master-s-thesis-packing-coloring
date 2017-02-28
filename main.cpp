#include <iostream>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>

#include "Cplex.hpp"
#include "Graph.hpp"

unsigned int A = 0; //Algorithm
const char* filename = NULL;
//random graph
unsigned int N = 0;
unsigned int P = 0;
//lattice graph
unsigned int R = 0;
unsigned int C = 0;

bool exists_test3 (const std::string& name);
void argParser(int argc, char const *argv[]);

int main(int argc, char const *argv[]) {
  //std::cout << (1 << 2) << std::endl;
  struct timeval end, start;
  unsigned long int timer;
  gettimeofday(&start, NULL);
  argParser(argc, argv);
  std::string outputFile;
  Graph* g = new Graph();
  if(filename == NULL) {
    if(N!=0 && P!=0) {
      outputFile = "random_"+ std::to_string(P) + "_" + std::to_string(N) +".txt";
      srand (time(NULL));
      g->generateRandomly(P,N);
    }if(R!=0 && C!=0) {
      outputFile = "lattice_"+ std::to_string(R) + "_" + std::to_string(C) +".txt";
      g->generateLatticeGraph(R, C);
    }
  }else {
    outputFile = filename;
    g->importCol(filename);
  }

  if(A == VERTEX_COLOR || A == VERTEX_COLOR_2) {
    outputFile += "modelVC.dat";
    g->exportText(outputFile.c_str());
  }else if(A == INDEPENDENT) {
    outputFile += ".cplex";
    if(!exists_test3(outputFile)) {
      g->findStableSet(0,0);
      g->exportTextStable(outputFile.c_str(),false,false);
    }
  }else if(A == BRANCH_AND_PRICE) {
    outputFile += "_BP.cplex";
    g->findStableSet(0,1);
    g->exportTextStable(outputFile.c_str(),false,true);
  }
  delete g;
  gettimeofday(&end, NULL);
  timer =  (1000000 * end.tv_sec + end.tv_usec) - (1000000 * start.tv_sec + start.tv_usec);
  std::cout << "Pre-processing time : " << timer << std::endl;

  gettimeofday(&start, NULL);
  Cplex cplex(outputFile.c_str(),A);
  cplex.modelise();
  gettimeofday(&end, NULL);
  timer =  (1000000 * end.tv_sec + end.tv_usec) - (1000000 * start.tv_sec + start.tv_usec);
  std::cout << "Cplex time : " << timer << std::endl;

  char* cmd = new char [200];
  cmd[0] = 0;
  strcat(cmd,"notify-send \" over with  ");
  strcat(cmd,outputFile.c_str());
  strcat(cmd," \"  ");
  system(cmd);

  return 0;
}

void argParser(int argc, char const *argv[]) {
  int i=1;
  while(i < argc) {
    if(strcmp(argv[i],"-a") == 0) {
      A = atoi(argv[i+1]);
    }else if(strcmp(argv[i],"-f") == 0) {
      filename = argv[i+1];
    }else if(strcmp(argv[i],"-N") == 0) {
      N = atoi(argv[i+1]);
    }else if(strcmp(argv[i],"-P") == 0) {
      P = atoi(argv[i+1]);
    }else if(strcmp(argv[i],"-R") == 0) {
      R = atoi(argv[i+1]);
    }else if(strcmp(argv[i],"-C") == 0) {
      C = atoi(argv[i+1]);
    }
    i++;
  }
  //std::cout << N << "  " << P << '\n';
  if( ! ( (A>0 && filename != NULL) || (A>0 && N != 0 && P!=0) || (A>0 && R!=0 && C!=0)  )   ) {
    std::cerr << "ERROR IN ARG" << '\n';
    exit( EXIT_FAILURE );
  }

}

bool exists_test3 (const std::string& name) {
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}
