#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>

int main (int argc, char* argv[]) {

  if (argc < 2) {
    std::cerr<<"usage: "<<argv[0]<<" <nbthreads>"<<std::endl;
    return -1;
  }
  
  // command line parameter
  int n;
  n = atoi(argv[1]);

  printf("I am thread %d in nbthreads", n);


	return 0;

}
