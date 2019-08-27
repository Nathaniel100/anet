#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ResolveInet(const char *addr, int port) {
  char port_str[6] = {0};  // strlen("65536") + 1
  snprintf(port_str, sizeof(port_str), "%d", port);
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = PF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  struct addrinfo *servinfo = nullptr;
  if (getaddrinfo(addr, port_str, &hints, &servinfo) != 0) {
    printf("getaddrinfo(%s, %s, hints, servinfo) failed: %s", addr, port_str,
           strerror(errno));
    return;
  }
  int maxlen = INET6_ADDRSTRLEN;
  char *s = (char *)malloc(maxlen + 1);
  memset(s, 0, maxlen + 1);
  for (struct addrinfo *p = servinfo; p; p = p->ai_next) {
    memset(s, 0, maxlen + 1);
    if (p->ai_family == AF_INET) {  // IPv4
      inet_ntop(AF_INET, &((struct sockaddr_in *)(p->ai_addr))->sin_addr, s,
                maxlen);
      printf("IPv4: %s\n", s);
    } else if (p->ai_family == AF_INET6) {  // IPv6
      inet_ntop(AF_INET6, &((struct sockaddr_in6 *)(p->ai_addr))->sin6_addr, s,
                maxlen);
      printf("IPv6: %s\n", s);
    }
  }
  free(s);
  freeaddrinfo(servinfo);
}

int main(int argc, char **argv) {
  ResolveInet("www.baidu.com", 0);
  return 0;
}
