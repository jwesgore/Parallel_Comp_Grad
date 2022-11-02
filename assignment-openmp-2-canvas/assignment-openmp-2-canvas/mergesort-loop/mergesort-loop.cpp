#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <omp.h>
#include <chrono>
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

  mergeSort(arr, left, mid);
  mergeSort(arr, mid + 1, right);
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

  if (argc < 3) { std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

  int n = atoi(argv[1]);
  int numThreads = atoi(argv[2]);

  // get arr data
  int * arr = new int [n];
  generateMergeSortData (arr, n);


  //insert sorting code here.
  // start timing
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

  omp_set_num_threads(numThreads);
  int leftsplits [numThreads];
  int rightsplits [numThreads];
  #pragma omp parallel
  {
    int thread = omp_get_thread_num();
    
    int left = -1, right = 0;
    #pragma omp for
    for (int i = 0; i < n; i++) {
      if (left == -1) left = i;
      right = i;
    }

    //std::cout<< left << " " << right << std::endl;

    #pragma omp for
    for (int i = 0; i < numThreads; i++) {
      mergeSort(arr, left, right);
    }
    
    leftsplits[thread] = left;
    rightsplits[thread] = right;
  }

  int groups = numThreads;
  int groupsize = 2;
  while (groups > 1) {

    if (groups % 2 == 0) groups = groups / 2;
    else groups = 1 + (groups / 2);
  
    #pragma omp parallel for
    for (int i = 0; i < groups; i++) {
      int left, right, mid;

      left = i * groupsize;  // left index of first subgroup 
      
      right = ((1+i) * groupsize) - 1; // right index of last subgroup
      if (right >= numThreads) right = numThreads - 1;
      
      mid = left + (groupsize / 2);   // right index of middle subgroup

      if (left != right) 
        merge(arr,leftsplits[left],rightsplits[right],leftsplits[mid] - 1);
    }
    groupsize = groupsize * 2;
  }

  // for (int i = 0; i < n ; i++) {
  //   std::cout << arr[i] << ", ";
  // }
  
  // std::cout<<std::endl;

  //printArray(arr, n);
  // end time
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapased_seconds = end-start;
  //Print the total execution time (in sec) to the error stream
  std::cerr<<elapased_seconds.count()<<std::endl;
  
  checkMergeSortResult (arr, n);

  delete[] arr;

  return 0;
}
