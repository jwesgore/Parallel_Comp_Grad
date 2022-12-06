#include <unistd.h>
#include <limits.h>
#include <stdio.h>

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 255
#endif

int main () {
  char hostname[HOST_NAME_MAX];
	gethostname(hostname, HOST_NAME_MAX);

  printf("Hostname: %s\n", hostname);
  return 0;
}
