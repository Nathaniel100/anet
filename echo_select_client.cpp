#include "anet.h"

#include <sys/select.h>
#include <sys/time.h>

#include <errno.h>
#include <stdio.h>

int main() {
  anet::SockAddr server_addr;
  const char *server_host = "127.0.0.1";
  uint16_t server_port = 9999;
  int ret = 0;
  if (anet::SockAddrSetV4(&server_addr, server_host, server_port) != 0) {
    printf("SockAddrSetV4(server_addr, %s, %d) failed\n", server_host,
           server_port);
    return -1;
  }
  int s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0) {
    printf("socket(AF_INET, SOCK_STREAM, 0) failed: %s\n", strerror(errno));
    return -1;
  }
  if (connect(s, &server_addr.sa.sa, SockAddrLen(&server_addr)) < 0) {
    close(s);
    printf("connect failed: %s\n", strerror(errno));
    return -1;
  }
  fd_set rdset;
  int maxfdp1 = s + 1;
  bool quit = false;
  for (;;) {
    FD_ZERO(&rdset);
    FD_SET(s, &rdset);
    if (!quit) {
      FD_SET(0, &rdset);
    }
    int result = select(maxfdp1, &rdset, nullptr, nullptr, nullptr);
    if (result < 0) {
      ret = -1;
      printf("select failed: %s\n", strerror(errno));
      goto end;
    }
    if (FD_ISSET(s, &rdset)) {
      char buffer[1024] = {0};
      int nread = read(s, buffer, sizeof(buffer));
      if (nread < 0) {
        ret = -1;
        printf("read failed: %s\n", strerror(errno));
        goto end;
      } else if (nread == 0) {
        ret = 0;
        printf("peer closed\n");
        goto end;
      } else {
        printf("read: %.*s\n", nread, buffer);
      }
    }
    if (FD_ISSET(0, &rdset)) {
      char buffer[1024] = {0};
      fgets(buffer, sizeof(buffer), stdin);
      buffer[strlen(buffer) - 1] = '\0';
      int buffer_length = strlen(buffer);
      if (buffer_length == 0) {
        continue;
      }
      if (strncmp(buffer, "quit", buffer_length) == 0) {
        quit = true;
        shutdown(s, SHUT_WR);
        continue;
      }
      int nwrite = write(s, buffer, buffer_length);
      if (nwrite <= 0) {
        ret = -1;
        printf("write failed: %s\n", strerror(errno));
        goto end;
      } else {
        printf("write: %.*s\n", nwrite, buffer);
      }
    }
  }

end:
  close(s);
  return ret;
}
