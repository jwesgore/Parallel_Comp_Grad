#include <chrono>
#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

// method to parse function 
typedef float (*ptr) (float,int);
ptr getFunction(int f) {
  switch (f) {
    case 1:
      return &f1;
    case 2:
      return &f2;
    case 3:
      return &f3;
    default:
      return &f4;
  }
}

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
  
  if (argc < 9) {
    std::cerr<<"Usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <scheduling> <granularity>"<<std::endl;
    return -1;
  }

  //insert code here

  // convert command line values
  int fid  =  atoi(argv[1]);
  int a    =  atoi(argv[2]);
  int b    =  atoi(argv[3]);
  int n    =  atoi(argv[4]);
  int intensity   =  atoi(argv[5]);
  int nbthreads   =  atoi(argv[6]);
  int granularity =  atoi(argv[8]);
  std::string scheduling  =  argv[7];

  // start clock and set threads
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  omp_set_num_threads(nbthreads);

  // other variables
  float (*ptr)(float, int) = getFunction(fid); // get function
  double co = (b - a) / float (n); // calculate coefficient
  if (granularity <= 0) granularity = 1;
  double sum = 0.0;

  if (scheduling == "dynamic") {
    #pragma omp parallel for schedule(dynamic, granularity) reduction(+:sum)
    for (int i = 0; i < n; i++)
      sum += (ptr)(a + ((i + .5) * co), intensity);
  } else {
    #pragma omp parallel for schedule(static, granularity) reduction (+:sum)
    for (int i = 0; i < n; i++)
      sum += (ptr)(a + ((i + .5) * co), intensity);
  }

  sum = sum * co;

  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;
  std::cout << sum << std::endl;
  std::cerr<<elapsed_seconds.count()<<std::endl;

  return 0;
}
