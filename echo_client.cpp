#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>

int ParseAddress(const char *host, int port, struct sockaddr_in *addr) {
  addr->sin_family = AF_INET;
  addr->sin_port = htons(port);
  if (inet_pton(AF_INET, host, &addr->sin_addr) < 0) {
    printf("inet_pton(AF_INET, %s) failed: %s\n", host, strerror(errno));
    return -1;
  }
  return 0;
}

int main() {
  int s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0) {
    printf("socket(AF_INET, SOCK_STREAM, 0) failed: %s\n", strerror(errno));
    return -1;
  }
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  if (ParseAddress("localhost", 9999, &addr) < 0) {
    printf("ParseAddress failed\n");
    return -1;
  }
  if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    printf("connect failed: %s\n", strerror(errno));
    return -1;
  }

  char buffer[1024] = "Hello World.";
  int nwrite = write(s, buffer, strlen(buffer));
  if (nwrite < 0) {
    printf("write failed: %s\n", strerror(errno));
    close(s);
    return -1;
  }
  printf("write success: %.*s\n", nwrite, buffer);
  memset(buffer, 0, sizeof(buffer));
  int nread = read(s, buffer, sizeof(buffer));
  if (nread == 0) {
    printf("peer has closed\n");
    close(s);
    return 0;
  } else if (nread < 0) {
    printf("read failed: %s\n", strerror(errno));
    close(s);
    return -1;
  }
  printf("read from server: %s\n", buffer);

  close(s);
  return 0;
}
