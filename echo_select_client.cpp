#include "anet.h"

#include <sys/select.h>
#include <sys/time.h>

#include <errno.h>

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
  fd_set rdset, wrset;
  int maxfdp1 = s + 1;
  FD_SET(s, &wrset);
  char buffer[1024] = {0};
  int nread = 0;
  bool want_read = false;
  bool want_write = true;
  for (;;) {
    FD_ZERO(&rdset);
    FD_ZERO(&wrset);
    if (want_read) {
      FD_SET(s, &rdset);
    }
    if (want_write) {
      FD_SET(s, &wrset);
    }
    int result = select(maxfdp1, &rdset, &wrset, nullptr, nullptr);
    if (result < 0) {
      ret = -1;
      printf("select failed: %s\n", strerror(errno));
      goto end;
    }
    if (FD_ISSET(s, &rdset)) {
      memset(buffer, 0, sizeof(buffer));
      nread = read(s, buffer, sizeof(buffer));
      if (nread < 0) {
        ret = -1;
        printf("read failed: %s\n", strerror(errno));
        goto end;
      } else if (nread == 0) {
        ret = 0;
        printf("peer closed\n");
        goto end;
      }
      printf("read: %.*s\n", nread, buffer);
      goto end;
    }
    if (FD_ISSET(s, &wrset)) {
      int nwrite = write(s, "Hello World", 11);
      if (nwrite <= 0) {
        printf("write failed: %s\n", strerror(errno));
        goto end;
      } else {
        printf("write: Hello World\n");
      }
      want_write = false;
      want_read = true;
    }
  }

end:
  close(s);
  return ret;
}
