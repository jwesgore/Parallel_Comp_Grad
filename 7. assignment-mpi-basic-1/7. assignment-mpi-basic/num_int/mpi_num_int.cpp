#include <iostream>
#include <cmath>
#include <stdio.h>
#include <cstdlib>
#include <chrono>
#include <mpi.h>

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

  // start timer
  auto start = std::chrono::system_clock::now();
  
  if (argc < 6) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<std::endl;
    return -1;
  }

  double result = 0.0; // init result
  float (*ptr)(float, int) = getFunction(atoi(argv[1])); // get function
  
  int a = atoi(argv[2]);
  int b = atoi(argv[3]);
  int n = atoi(argv[4]);
  int intensity = atoi(argv[5]);
  float co =  (b - a) / float (n); // calculate coefficient
  
  // MPI start
  MPI_Init(&argc, &argv);
  int size, rank;

  // start timer
  //std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // get separation
  int loop_start = rank * (n / size);
  int loop_end = (rank + 1) * (n / size);
  if (size - 1 == rank) loop_end = n;

  double rank_val = 0.0;

  // calculate all values
  for (int i = loop_start; i < loop_end; i++) {
    rank_val += (*ptr)(a + ((i + .5) * co), intensity);
  }
  
  rank_val *= co;

  // bring all calculations together
  MPI_Reduce(&rank_val, &result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  MPI_Finalize();

  // print on rank 0
  if (rank == 0) {
    // get runtime
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;

    // print results
    std::cout << result << std::endl;
    std::cerr << elapsed_seconds.count() << std::endl;
  }

  return 0;
}
