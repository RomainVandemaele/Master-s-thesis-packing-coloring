#include "common.hpp"

void myFscanf(FILE* file, const char* type, void* var ) {
  int r = fscanf(file,type,var);
  if( r == EOF) {
    std::cerr << "FSCANF FAILED" << '\n';
  }
}
