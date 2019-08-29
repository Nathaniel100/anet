#include "anet.h"

namespace anet {

int SockAddrLen(const SockAddr *sa) {
  switch (sa->family) {
    case AF_INET:
      return sizeof(struct sockaddr_in);
    case AF_INET6:
      return sizeof(struct sockaddr_in6);
    default:
      return sizeof(struct sockaddr);
  }
}

int SockAddrSetV4(SockAddr *sa, const char *host, uint16_t port) {
  int res = 1;
  memset(sa, 0, sizeof(*sa));
  // 如果为host为null，默认是为0，与以下效果一样
  // sa->sa.v4.sin_addr.s_addr = INADDR_ANY;
  if (host) {
    res = inet_pton(AF_INET, host, &sa->sa.v4.sin_addr);
  }
  if (res > 0) {
    sa->sa.v4.sin_family = AF_INET;
    sa->sa.v4.sin_port = htons(port);
    sa->family = AF_INET;
    return 0;
  }
  return -1;
}

int SockAddrSetV6(SockAddr *sa, const char *host, uint16_t port) {
  memset(sa, 0, sizeof(*sa));
  if (!host) {
    sa->sa.v6.sin6_family = AF_INET6;
    sa->sa.v6.sin6_port = htons(port);
    sa->family = AF_INET6;
    return 0;
  }
  char port_str[10] = {0};
  snprintf(port_str, sizeof(port_str), "%d", port);

  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_flags = AI_NUMERICHOST;
  hints.ai_family = AF_INET6;

  struct addrinfo *ai = nullptr;
  if (getaddrinfo(host, port_str, &hints, &ai) != 0) {
    return -1;
  }
  int result = SockAddrSetAddrInfo(sa, ai);
  freeaddrinfo(ai);
  return result;
}

int SockAddrSetAddrInfo(SockAddr *sa, const struct addrinfo *ai) {
  memset(sa, 0, sizeof(*sa));
  sa->family = ai->ai_family;
  switch (ai->ai_family) {
    case AF_INET:
      memcpy(&sa->sa.sa, ai->ai_addr, sizeof(sa->sa.v4));
      return 0;
    case AF_INET6:
      memcpy(&sa->sa.sa, ai->ai_addr, sizeof(sa->sa.v6));
      return 0;
    default:
      return -1;
  }
}

}  // namespace anet
