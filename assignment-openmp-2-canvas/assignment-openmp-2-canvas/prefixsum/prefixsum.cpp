#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <cstring>
#include <chrono>


using namespace std;

#ifdef __cplusplus
extern "C" {
#endif
  void generatePrefixSumData (int* arr, size_t n);
  void checkPrefixSumResult (int* arr, size_t n);
#ifdef __cplusplus
}
#endif

int main (int argc, char* argv[]) {
  //forces openmp to create the threads beforehand
#pragma omp parallel
  {
      int fd = open (argv[0], O_RDONLY);
      if (fd != -1) {
        close (fd);
      }
      else {
        std::cerr<<"something is amiss"<<std::endl;
      }
  }

  if (argc < 3) {
    std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }


  int n = atoi(argv[1]);
  int nbThreads = atoi(argv[2]);
  int * arr = new int [n];
  generatePrefixSumData (arr, n);

  int * pr = new int [n+1];

  //insert prefix sum code here
  
  // initialize some values
  omp_set_num_threads(nbThreads);
  int threadPartialSums [nbThreads];
  
  #pragma omp parallel 
  {
    // initialize some values
    int thread = omp_get_thread_num();
    int threadPartialSum = 0;
    int fix = 0;

    // set all values to 0 in pr
    #pragma omp for
    for (int i = 0; i < n; i++) {
      pr[i] = 0;
    }

    // calculate partial prefix sums
    #pragma omp for schedule(static) nowait
    for (int i = 0; i < n; i++) {
      threadPartialSum += arr[i];
      pr[i + 1] = threadPartialSum; 
    }

    threadPartialSums[thread] = threadPartialSum; // store values in shared array

    #pragma omp barrier // wait before fixing
    
    //calculate fix
    for (int i = thread; i > 0; i--) {
      fix += threadPartialSums[i - 1];
    }
    
    // apply fix
    #pragma omp for schedule(static) nowait
    for (int i = 0; i < n; i++) {
      pr[i + 1] += fix;
    }

  }

  
  // start timing
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();


  // end time
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapased_seconds = end-start;
  //Print the total execution time (in sec) to the error stream
  cerr<<elapased_seconds.count()<<std::endl;

  checkPrefixSumResult(pr, n);

  delete[] arr;

  return 0;
}
