#include <math.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif
    
    float f1(float x, int intensity);
    float f2(float x, int intensity);
    float f3(float x, int intensity);
    float f4(float x, int intensity);
    
#ifdef __cplusplus
}
#endif

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

double calc_numerical_integration(int functionid, int a, int b, int n, int intensity){
    
    float (*ptr)(float, int) = getFunction(functionid); // get function
    float co =  (b - a) / (float) (n); // calculate coefficient
    double result = 0;

    for (int i = 0; i < n; i++) {
        result += (*ptr)(a + ((i + .5) * co), intensity);
    }

    return result * co;
}

int main (int argc, char* argv[]) {
    
    if (argc < 6) {
        fprintf(stderr, "usage: %s <functionid> <a> <b> <n> <intensity>", argv[0]);
        return -1;
    }

    // convert command line values
    int function_id  =  atoi(argv[1]);
    int a    =  atoi(argv[2]);
    int b    =  atoi(argv[3]);
    int n    =  atoi(argv[4]);
    int intensity =  atoi(argv[5]);
    
    clock_t t; // t represents clock ticks which is of type 'clock_t'
    t = clock(); // start clock
    double r = calc_numerical_integration(function_id, a, b, n, intensity);
    t = clock()-t; // end clock=
    float time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds; CLOCKS_PER_SEC is the number of clock ticks per second
    printf("%lf\n", r);
    fprintf(stderr, "%f\n", time_taken);
    
    return 0;
}

