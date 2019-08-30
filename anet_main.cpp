#include "anet.h"

void Usage(const char *program) { printf("Usage: %s 4|6 <HOST> <PORT>\n", program); }

int TestStatic() {
  anet::SockAddr addr;
  char buffer[1024] = {0};
  const char *host_v4 = "192.168.203.203";
  const char *host_v6 = "fe80::865:9f9e:95e9:2766";
  uint16_t port = 8999;
  if (anet::SockAddrSetV4(&addr, host_v4, port) != 0) {
    printf("SockAddrSetV4(%s, %d) failed\n", host_v4, port);
    return -1;
  }
  if (anet::SockAddrPrint(buffer, sizeof(buffer), &addr, true) < 0) {
    printf("SockAddrPrint failed\n");
    return -1;
  }
  printf("IPv4 %s:%d => %s\n", host_v4, port, buffer);

  memset(&addr, 0, sizeof(addr));
  memset(buffer, 0, sizeof(buffer));
  if (anet::SockAddrSetV6(&addr, host_v6, port) != 0) {
    printf("SockAddrSetV6(%s, %d) failed\n", host_v6, port);
    return -1;
  }
  if (anet::SockAddrPrint(buffer, sizeof(buffer), &addr, true) < 0) {
    printf("SockAddrPrint failed\n");
    return -1;
  }
  printf("IPv6 %s:%d => %s\n", host_v6, port, buffer);
  return 0;
}

int PrintAddress(const char *host, int port, int v) {
  anet::SockAddr addr;
  char buffer[1024] = {0};
  if (v == 4) {
    if (anet::SockAddrSetV4(&addr, host, port) != 0) {
      printf("SockAddrSetV4(%s, %d) failed\n", host, port);
      return -1;
    }
  } else {
    if (anet::SockAddrSetV6(&addr, host, port) != 0) {
      printf("SockAddrSetV6(%s, %d) failed\n", host, port);
      return -1;
    }
  }
  if (anet::SockAddrPrint(buffer, sizeof(buffer), &addr, true) < 0) {
    printf("SockAddrPrint failed\n");
    return -1;
  }
  printf("%s\n", buffer);
  return 0;
}

int main(int argc, char **argv) {
  if (argc != 4) {
    Usage(argv[0]);
    return -1;
  }
  int v = strtol(argv[1], nullptr, 0);
  if (v != 4 && v != 6) {
    Usage(argv[0]);
    return -1;
  }
  const char *host = argv[2];
  int port = strtol(argv[3], nullptr, 0);
  if (port <= 0) {
    Usage(argv[0]);
    printf("port(%d) should be positive\n", port);
    return -1;
  }
  PrintAddress(host, port, v);
  return 0;
}
