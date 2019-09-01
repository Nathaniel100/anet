#include <sys/time.h>
#include <sys/types.h>

#include <stdio.h>
#include <string.h>

int main() {
  struct timeval tv;
  memset(&tv, 0, sizeof(tv));
  printf("sizeof(timeval) = %d\n", (int)sizeof(tv));
  printf("sizeof(tv_sec) = %d\n", (int)sizeof(tv.tv_sec));
  printf("sizeof(tv_usec) = %d\n", (int)sizeof(tv.tv_usec));

  return 0;
}
