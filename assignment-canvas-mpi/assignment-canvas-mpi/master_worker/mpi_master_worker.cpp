#include <mpi.h>
#include <chrono>
#include <iostream>

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

// master method
double master(int size, int n) {

  MPI_Status status;

  int work_running    = 0;
  int position        = 0;
  double result       = 0.0;
  double rank_result  = 0.0;

  // get granularity
  int granularity = 50;
  if (granularity < 1) granularity = 1;

  // send initial batch of work
  for (int i = 1; i < size; i++) {
    if (position >= n) break; // make sure not to send too much work

    int loop[] = {position, position + granularity}; // get loop start and end
    MPI_Send(loop, 2, MPI_INT, i, 0, MPI_COMM_WORLD); // send first job
    work_running++;
    position += granularity; // adjust position
  }

  // send further batches of work
  while (work_running > 0) {
   
    MPI_Recv(&rank_result, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status); // receive value
    work_running--;

    result += rank_result; // add value into result

    if (position < n) {
      int rank_id = status.MPI_SOURCE; // get which rank to send to

      for (int i = 0; i < 3; i++) {
        if (position >= n) break; // break if position gets too large
        int loop[] = {position, position + granularity}; // get loop start and end
        MPI_Send(loop, 2, MPI_INT, rank_id, 0, MPI_COMM_WORLD); // send next job
        work_running++;

        position+= granularity; // adjust position
      }
    }
  }

  for (int i = 1; i < size; i++) {
    MPI_Send(0, 0, MPI_INT, i, 69, MPI_COMM_WORLD); // send ending message
  }

  return result;
}

// worker method
void worker(int fid, float co, int a, int intensity, int n){

  MPI_Status status;
  int loop[] = {0};

  float (*ptr)(float, int) = getFunction(fid); // get function

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
  while (1) {
    
    // get start and end and store in loop[]
    MPI_Recv(loop, 2, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    if (status.MPI_TAG == 69) {return;} // end if no work
    else { 
      
      // calculate all values
      double rank_val = 0.0;
      if (loop[1] > n) loop[1] = n; 

      for (int i = loop[0]; i < loop[1]; i++) {
        rank_val += (*ptr)(a + ((i + .5) * co), intensity);
      }

      rank_val *= co;

      MPI_Send(&rank_val, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD); // send back results
    }
  }
}


int main (int argc, char* argv[]) {

  if (argc < 6) {
    std::cerr<<"usage: mpirun "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<std::endl;
    return -1;
  }

  // start timer
  auto start = std::chrono::system_clock::now();

  double result = 0.0; // init result
  int fid = atoi(argv[1]); // get function
   
  int a = atoi(argv[2]);
  int b = atoi(argv[3]);
  int n = atoi(argv[4]);
  int intensity = atoi(argv[5]);
  float co =  (b - a) / float (n); // calculate coefficient

  // MPI start
  MPI_Init(&argc, &argv);
  int size, rank;

  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) result = master(size, n);
  else worker(fid, co, a, intensity, n);

  // print on rank 0
  if (rank == 0) {
    // get runtime
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;

    // print results
    std::cout << result << std::endl;
    std::cerr << elapsed_seconds.count() << std::endl;
  }

  MPI_Finalize();

  return 0;
}
