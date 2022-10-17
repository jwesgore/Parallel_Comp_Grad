#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>


#ifdef __cplusplus
extern "C" {
#endif
    void generateReduceData (int* arr, size_t n);
#ifdef __cplusplus
}
#endif


int main (int argc, char* argv[]) {
    //forces openmp to create the threads beforehand

    long sum = 0;
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

    generateReduceData (arr, atoi(argv[1]));
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    omp_set_num_threads(threads);

    // insert reduction code here

    if (granularity <= 0) granularity = 1;
    
    if (scheduling == "dynamic") {
        #pragma omp parallel for schedule(dynamic, granularity) reduction(+:sum)
        for (int i = 0; i < n; i++) {
            sum += arr[i];
        }
    } else {
        #pragma omp parallel for schedule(static, granularity) reduction(+:sum)
        for (int i = 0; i < n; i++)
            sum += arr[i];
    }
    
    
    
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end-start;
    
    std::cout << sum << std::endl;
    
    std::cerr<<elapsed_seconds.count()<<std::endl;

    delete[] arr;
    
    return 0;
}
