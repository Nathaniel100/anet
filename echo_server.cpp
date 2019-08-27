#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>

void PrintAddress(const struct sockaddr_in *addr) {
  char buffer[128] = {0};
  if (inet_ntop(AF_INET, &addr->sin_addr, buffer, sizeof(buffer)) == NULL) {
    printf("inet_ntop failed: %s\n", strerror(errno));
    return;
  }
  printf("Address: %s:%d\n", buffer, ntohs(addr->sin_port));
}

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
    printf("ParseAddress failed");
    return -1;
  }

  if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    printf("bind failed:%s\n", strerror(errno));
    return -1;
  }

  if (listen(s, 128) < 0) {
    printf("listen failed: %s\n", strerror(errno));
    return -1;
  }

  for (;;) {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    memset(&client_addr, 0, sizeof(client_addr));
    int connected_socket = accept(s, (struct sockaddr *)&client_addr, &client_addr_len);
    if (connected_socket < 0) {
      printf("accept failed: %s\n", strerror(errno));
      continue;
    }
    PrintAddress(&client_addr);
    char buffer[1024] = {0};
    int nread = read(connected_socket, buffer, sizeof(buffer));
    if (nread < 0) {
      printf("read failed: %s\n", strerror(errno));
      close(connected_socket);
      continue;
    } else if (nread == 0) {
      printf("peer has closed\n");
      close(connected_socket);
      continue;
    }
    printf("read from client: %.*s\n", nread, buffer);
    int nwrite = write(connected_socket, buffer, nread);
    if (nwrite < 0) {
      printf("write failed: %s\n", strerror(errno));
      close(connected_socket);
      continue;
    }
    printf("write success: %.*s\n", nwrite, buffer);
    close(connected_socket);
  }

  close(s);
  return 0;
}
