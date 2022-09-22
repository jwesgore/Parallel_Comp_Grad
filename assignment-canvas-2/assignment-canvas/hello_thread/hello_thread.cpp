#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// print method
void *f(void *p) {

  int pt;
  pt = *(int *) p;

  printf("I am thread %d in nbthreads \n", pt);
  return NULL;
}

int main (int argc, char* argv[]) {

  if (argc < 2) {
    std::cerr<<"usage: "<<argv[0]<<" <nbthreads>"<<std::endl;
    return -1;
  }
  
  // command line parameter
  int n;
  n = atoi(argv[1]);

  // initialize variables
  pthread_t threads[n];
  long taskids[n];

  //create threads
  for (int i = 0; i < n; ++i) {
    taskids[i] = i;
    pthread_create(&threads[i], NULL, f, (void*)&taskids[i]);
  }

  // join threads
  for (int i = 0; i < n; ++i) {
    pthread_join(threads[i], NULL);
  }

	return 0;
}
