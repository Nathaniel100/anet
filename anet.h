#ifndef ANET_H
#define ANET_H

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

namespace anet {

struct SockAddr {
  union {
    struct sockaddr sa;
    struct sockaddr_in v4;
    struct sockaddr_in6 v6;
  } sa;
  int family;
};

int SockAddrLen(const SockAddr *sa);
int SockAddrSetV4(SockAddr *sa, const char *host, uint16_t port);
int SockAddrSetV6(SockAddr *sa, const char *host, uint16_t port);
int SockAddrSetAddrInfo(SockAddr *sa, const struct addrinfo *ai);

}

#endif
