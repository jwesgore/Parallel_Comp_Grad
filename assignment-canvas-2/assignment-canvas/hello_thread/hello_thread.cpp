#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>

int main (int argc, char* argv[]) {

  if (argc < 2) {
    std::cerr<<"usage: "<<argv[0]<<" <nbthreads>"<<std::endl;
    return -1;
  }
  
  char hostname[HOST_NAME_MAX + 1];
	gethostname(hostname, HOST_NAME_MAX + 1);

	printf("%s\n", hostname);

	return 0;

}
