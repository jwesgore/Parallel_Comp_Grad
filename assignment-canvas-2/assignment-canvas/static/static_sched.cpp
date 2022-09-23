#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>
#include <cmath>
#include <pthread.h>

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

pthread_mutex_t mu;

// structure
  struct mStruct{
    int a;
    float co;
    ptr func;
    int intensity;
    float* result;
    int start;
    int end;
  };

void* thread(void* p) {
  mStruct temp;
  temp = *(mStruct *) p;
  float y = 0;

  for (int i = temp.start; i < temp.end; i++) {
    float x = (*temp.func)(temp.a + ((i + .5) * temp.co), temp.intensity);
    y += x;
  }

  pthread_mutex_lock(&mu);
  *temp.result += y * temp.co;
  pthread_mutex_unlock(&mu);
  return 0;
}

void* iter(void* p) {
  mStruct temp;
  temp = *(mStruct *) p;

  for (int i = temp.start; i < temp.end; i++) {
    float x = (*temp.func)(temp.a + ((i + .5) * temp.co), temp.intensity);
    pthread_mutex_lock(&mu);
    *temp.result += x * temp.co;
    pthread_mutex_unlock(&mu);
  }
  return 0;
}

int main (int argc, char* argv[]) {

  if (argc < 8) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <sync>"<<std::endl;
    return -1;
  }
    
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

  // do your calculation here

  // convert command line values
  int fid  =  atoi(argv[1]);
  int a    =  atoi(argv[2]);
  int b    =  atoi(argv[3]);
  int n    =  atoi(argv[4]);
  int intensity =  atoi(argv[5]);
  int nbthreads =  atoi(argv[6]);
  std::string sync       =  argv[7];
  const std::string type = "thread";

  // other variables
  float (*ptr)(float, int) = getFunction(fid); // get function
  float co =  (b - a) / float (n); // calculate coefficient
  float result;
  pthread_t threads[nbthreads];
  mStruct threadData[nbthreads];
  mStruct threadDataTemplate = {a, co, ptr, intensity, &result,0,1};

  int spread = n / nbthreads;
  int remainder   = n % nbthreads;

  
  //create threads
  int point = 0;
  for (int i = 0; i < nbthreads; ++i) {
    
    threadData[i] = threadDataTemplate;
    threadData[i].start = point;

    if (remainder > 0) {
      remainder--;
      point++;
    }
    
    point += spread;
    threadData[i].end = point;
    if (sync.compare(type) == 0) 
      pthread_create(&threads[i], NULL, thread, (void*)&threadData[i]);
    else
      pthread_create(&threads[i], NULL, iter, (void*)&threadData[i]);
  }

  // join threads
  for (int i = 0; i < nbthreads; ++i) {
    pthread_join(threads[i], NULL);
  }
  
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;

  // report reult and time
  std::cout<<result<<std::endl;
  std::cerr<<elapsed_seconds.count()<<std::endl;
  
  return 0;
}
