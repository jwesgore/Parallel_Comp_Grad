#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>

struct timeval start, end;

#ifdef __cplusplus
extern "C" {
#endif
    void generateReduceData (int* arr, size_t n);
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
    
    if (argc < 5) {
        std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads> <scheduling> <granularity>"<<std::endl;
        return -1;
    }
    
    int n = atoi(argv[1]);
    int * arr = new int [n];
    int threads = atoi(argv[2]);
    char *scheduling = argv[3];
    int granularity = atoi(argv[4]);
    int sum = 0;

    generateReduceData (arr, atoi(argv[1]));
    
    gettimeofday(&start, NULL);

    // insert reduction code here
    if (granularity <= 0) granularity = 1;

    if (scheduling == "dynamic") {
        #pragma omp parallel for schedule(dynamic, granularity) num_threads(threads)
        for (int i = 0; i < n; i++) {
            sum += arr[i];
        }
    } else {
        #pragma omp parallel for schedule(static, granularity) num_threads(threads)
        for (int i = 0; i < n; i++) {
            sum += arr[i];
        }
    }
    
    gettimeofday(&end, NULL);
    double last_time = (end.tv_sec-start.tv_sec + (end.tv_usec - start.tv_usec)/1000000);
    std::cout << sum << std::endl;
    std::cerr << last_time << std::endl;

    delete[] arr;
    
    return 0;
}
