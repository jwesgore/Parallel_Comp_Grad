#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <chrono>
#include <omp.h>
#include <string.h>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

  void generateMergeSortData (int* arr, size_t n);
  void checkMergeSortResult (int* arr, size_t n);

#ifdef __cplusplus
}
#endif

void merge(int* arr, int left, int right, int mid) {
  
  std::vector <int> leftVector, rightVector;
  int lposition = 0, rposition = 0 , tposition = left;

  for (int i = left; i <= mid; i++) {
    leftVector.push_back(arr[i]);
  }
  for (int i = mid + 1; i <= right; i++) {
    rightVector.push_back(arr[i]);
  }

  while (lposition < leftVector.size() && rposition < rightVector.size()) {
    
    if (leftVector[lposition] < rightVector[rposition]) {
      arr[tposition] = leftVector[lposition];
      lposition++;
    } else {
      arr[tposition] = rightVector[rposition];
      rposition++;
    }
    tposition++;
  }

  while(lposition < leftVector.size()) {
    arr[tposition] = leftVector[lposition];
    lposition++, tposition++;
  }

  while(rposition < rightVector.size()) {
    arr[tposition] = rightVector[rposition];
    rposition++, tposition++;
  }

}

void mergeSort(int* arr, int left, int right) {
  
  int mid = (left + right) / 2;
  
  if (left >= right) return;

  #pragma omp task shared(arr)
  mergeSort(arr, left, mid);

  #pragma omp task shared(arr)
  mergeSort(arr, mid + 1, right);

  #pragma omp taskwait
  merge(arr, left, right, mid);

}

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

  if (argc < 3) { std::cerr<<"usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

  int n = atoi(argv[1]);
  int numThreads = atoi(argv[2]);

  // get arr data
  int * arr = new int [n];
  int * tmp = new int [n];

  generateMergeSortData (arr, n);
  //printArray(arr,n);

  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  //insert sorting code here.

  omp_set_num_threads(numThreads);
  #pragma omp parallel
  {
    #pragma omp single
    {
      mergeSort(arr, 0, n-1);
    }
  }

  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapased_seconds = end-start;
  //Print the total execution time (in sec) to the error stream
  std::cerr<<elapased_seconds.count()<<std::endl;
  //printArray(arr,n);
  checkMergeSortResult (arr, n);


  delete[] arr;

  return 0;
}
