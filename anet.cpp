#include "anet.h"

#include <algorithm>

namespace anet {

namespace {
inline uint32_t Min(uint32_t a, uint32_t b) { return a < b ? a : b; }
}  // namespace

int SockAddrLen(const SockAddr *sa) {
  switch (sa->family) {
    case AF_INET:
      return sizeof(sa->sa.v4);
    case AF_INET6:
      return sizeof(sa->sa.v6);
    default:
      return sizeof(sa->sa.sa);
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
  char port_str[10] = {0};  // 最大长度: strlen("65536") + 1
  snprintf(port_str, sizeof(port_str), "%d", port);

  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_flags = AI_NUMERICHOST;
  hints.ai_family = AF_INET6;

  struct addrinfo *ai = nullptr;
  if (getaddrinfo(host, port_str, &hints, &ai) != 0) {
    return -1;
  }
  sa->family = ai->ai_family;
  switch (ai->ai_family) {
    case AF_INET:
      memcpy(&sa->sa.sa, ai->ai_addr, sizeof(sa->sa.v4));
      freeaddrinfo(ai);
      return 0;
    case AF_INET6:
      memcpy(&sa->sa.sa, ai->ai_addr, sizeof(sa->sa.v6));
      freeaddrinfo(ai);
      return 0;
    default:
      freeaddrinfo(ai);
      return -1;
  }
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

int SockAddrSetPort(SockAddr *sa, uint16_t port) {
  switch (sa->family) {
    case AF_INET:
      sa->sa.v4.sin_port = htons(port);
      return 0;
    case AF_INET6:
      sa->sa.v6.sin6_port = htons(port);
      return 0;
    default:
      return -1;
  }
}

uint16_t SockAddrGetPort(const SockAddr *sa) {
  switch (sa->family) {
    case AF_INET:
      return ntohs(sa->sa.v4.sin_port);
    case AF_INET6:
      return ntohs(sa->sa.v6.sin6_port);
    default:
      return 0;
  }
}

int SockAddrPrint(char *buffer, uint32_t buffer_size, const SockAddr *sa,
                  bool want_port) {
  // 字符串不会超过: IPv6的地址长度+ 1(" ")+端口长度(最多strlen(65536))
  char local_buffer[INET6_ADDRSTRLEN + 10] = {0};
  uint32_t local_len = 0;
  const char *res = nullptr;
  switch (sa->family) {
    case AF_INET:
      res = inet_ntop(sa->family, &sa->sa.v4.sin_addr, local_buffer,
                      sizeof(sa->sa.v4));
      break;
    case AF_INET6:
      res = inet_ntop(sa->family, &sa->sa.v6.sin6_addr, local_buffer,
                      sizeof(sa->sa.v6));
      break;
  }
  if (!res) {
    return -1;
  }
  local_len = strlen(local_buffer);
  if (want_port) {
    local_len +=
        snprintf(local_buffer + local_len, sizeof(local_buffer) - local_len,
                 " %d", SockAddrGetPort(sa));
  }
  memcpy(buffer, local_buffer, Min(buffer_size, local_len));
  return Min(buffer_size, local_len);
}

}  // namespace anet
