#include <iostream>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

pthread_mutex_t mu1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mu2 = PTHREAD_MUTEX_INITIALIZER;

// structure
struct mStruct{
  int a;
  int n;
  double co;
  ptr func;
  int intensity;
  double* result;
  int* point;
  int granularity;
};

void* thread(void* p) {

  mStruct temp;
  temp = *(mStruct *) p;
  double y = 0;
  int start = 0,end = 0;

  while (true)  {
    
    pthread_mutex_lock(&mu1);
    start = *temp.point;
    *temp.point += temp.granularity;
    end = *temp.point;
    pthread_mutex_unlock(&mu1);
    
    if (start > temp.n) break;
    if (end > temp.n) end = temp.n;

    for (int i = start; i < end; i++) {
      double x = (*temp.func)(temp.a + ((i + .5) * temp.co), temp.intensity);
      y += x;
    }
    
  }
  
  pthread_mutex_lock(&mu2);
  *temp.result += y * temp.co;
  pthread_mutex_unlock(&mu2);
  return 0;
}

void* chunk(void* p) {
  mStruct temp;
  temp = *(mStruct *) p;
  int start, end;

  while (true)  {
    
    double y = 0;
    
    pthread_mutex_lock(&mu1);
    start = *temp.point;
    *temp.point += temp.granularity;
    end = *temp.point;
    pthread_mutex_unlock(&mu1);
    
    if (start > temp.n) break;
    if (end > temp.n) end = temp.n;

    for (int i = start; i < end; i++) {
      double x = (*temp.func)(temp.a + ((i + .5) * temp.co), temp.intensity);
      y += x;
    }

    pthread_mutex_lock(&mu2);
    *temp.result += y * temp.co;
    pthread_mutex_unlock(&mu2);
  }

  return 0;
}

void* iter(void* p) {
  mStruct temp;
  temp = *(mStruct *) p;
  int start, end;

  while (true)  {
    
    pthread_mutex_lock(&mu1);
    start = *temp.point;
    *temp.point += temp.granularity;
    end = *temp.point;
    pthread_mutex_unlock(&mu1);
    
    if (start > temp.n) break;
    if (end > temp.n) end = temp.n;

    for (int i = start; i < end; i++) {
      double x = (*temp.func)(temp.a + ((i + .5) * temp.co), temp.intensity);
      pthread_mutex_lock(&mu2);
      *temp.result += x * temp.co;
      pthread_mutex_unlock(&mu2);
    }
  }

  return 0;
}

int main (int argc, char* argv[]) {

  if (argc < 9) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <sync> <granularity>"<<std::endl;
    return -1;
  }
    
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

  // do your calculation here

  // convert command line values
  int fid  =  atoi(argv[1]);
  int a    =  atoi(argv[2]);
  int b    =  atoi(argv[3]);
  int n    =  atoi(argv[4]);
  int intensity   =  atoi(argv[5]);
  int nbthreads   =  atoi(argv[6]);
  int granularity =  atoi(argv[8]);
  std::string sync        =  argv[7];
  const std::string type1 = "thread";
  const std::string type2 = "chunk";

  // other variables
  float (*ptr)(float, int) = getFunction(fid); // get function
  double co =  (b - a) / float (n); // calculate coefficient
  double result = 0;
  int point = 0;
  pthread_t threads[nbthreads];
  mStruct threadData[nbthreads];
  mStruct threadDataTemplate = 
    {a, n, co, ptr, intensity, &result, &point, granularity};
  
  //create threads
  for (int i = 0; i < nbthreads; ++i) {
    threadData[i] = threadDataTemplate;
    if (sync.compare(type1) == 0) 
      pthread_create(&threads[i], NULL, thread, (void*)&threadData[i]);
    else if (sync.compare(type2) == 0)
      pthread_create(&threads[i], NULL, chunk, (void*)&threadData[i]);
    else
      pthread_create(&threads[i], NULL, iter, (void*)&threadData[1]);
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
