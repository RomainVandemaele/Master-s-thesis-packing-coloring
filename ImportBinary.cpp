#include <iostream>
#include <fstream>
#include <string.h>
#include "Graph.hpp"

int main(int argc, char const *argv[]) {
  //std::cout << (1 << 2) << std::endl;
  char line[500];
  int N =0, I= 0, D = 0;
  FILE * file;
  file = fopen( "instances/DSJC125.5.col.dat" , "r");
  fscanf(file, "%i", &N);
  fscanf(file, "%i", &D);
  fscanf(file, "%i", &I);
  std::cout << N << std::endl;
  std::cout << D << std::endl;
  std::cout << I << std::endl;

  int m = 0;
  for (size_t i = 0; i < I; i++) {
    fscanf(file, "%i", &m);
    std::cout << m << " ";
  }
  std::cout << std::endl;
  unsigned char c;
  //pass two bytes(espace ?)
  fscanf(file, "%c", &c);
  fscanf(file, "%c", &c);
  int s = N/8 + 1;
  for (size_t j = 0; j < I; j++) {
    short unsigned int mask = 128;
    int counter = 0;
    for (size_t k = 0; k < s; k++) {
      fscanf(file, "%c", &c);
      //std::cout << "k : " << c << std::endl;
      for (size_t l = 0; l < 8; l++) {
        if(counter < N) {
          bool bit = ( c & mask ) > 0;
          std::cout << bit;
        }
        mask = mask >> 1;
        counter++;
      }
      mask = 128;
    }
    //fscanf(file, "%c", &c);
    fscanf(file, "%c", &c); //read '\n'
    std::cout << std::endl;
  }
  std::cout  << std::endl;
  fclose(file);
  return 0;
}
