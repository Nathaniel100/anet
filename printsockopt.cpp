#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>

#define LEVEL_END 0x12345678

struct SockOpt {
  int opt_level;
  int opt_name;
  int opt_value_size;
  const char *description;
};

const SockOpt ALL_SOCK_OPTS[] = {
    {SOL_SOCKET, SO_REUSEADDR, sizeof(int), "地址复用"},
    {SOL_SOCKET, SO_REUSEPORT, sizeof(int), "端口复用"},
    {SOL_SOCKET, SO_RCVBUF, sizeof(int), "接收缓冲区大小"},
    {SOL_SOCKET, SO_SNDBUF, sizeof(int), "发送缓冲区大小"},
    {SOL_SOCKET, SO_KEEPALIVE, sizeof(int), "周期测试连接是否存在"},
};
const int ALL_SOCK_OPTS_SIZE = sizeof(ALL_SOCK_OPTS) / sizeof(ALL_SOCK_OPTS[0]);

int main() {
  int s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0) {
    printf("socket(AF_INET, SOCK_STREAM, 0) failed: %s\n", strerror(errno));
    return -1;
  }
  for (int i = 0; i < ALL_SOCK_OPTS_SIZE; i++) {
    const SockOpt *opt = &ALL_SOCK_OPTS[i];
    int opt_value_size = opt->opt_value_size;
    int opt_value = 0;
    if (getsockopt(s, opt->opt_level, opt->opt_name, (void *)&opt_value,
                   (socklen_t *)&opt_value_size) < 0) {
      printf("getsockopt (%s) failed: %s\n", opt->description, strerror(errno));
    }
    printf("%s: %d\n", opt->description, opt_value);
  }
  close(s);
  return 0;
}
