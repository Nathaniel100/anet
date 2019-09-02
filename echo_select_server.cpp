#include "anet.h"

#include <errno.h>

#define IMAX(a, b) ((a < b) ? (b) : (a))
#define LISTENQ 128

int main() {
  const char *server_host = "127.0.0.1";
  uint16_t server_port = 9999;
  anet::SockAddr server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  if (anet::SockAddrSetV4(&server_addr, server_host, server_port) != 0) {
    printf("SockAddrSetV4(server_addr, %s, %d) failed\n", server_host,
           server_port);
  }

  int s = socket(server_addr.family, SOCK_STREAM, 0);
  if (s < 0) {
    printf("socket(AF_INET, SOCK_STREAM, 0) failed: %s\n", strerror(errno));
    return -1;
  }
  if (bind(s, &server_addr.sa.sa, anet::SockAddrLen(&server_addr)) != 0) {
    printf("bind failed: %s\n", strerror(errno));
    close(s);
    return -1;
  }
  if (listen(s, LISTENQ) != 0) {
    printf("listen(s, %d) failed: %s\n", LISTENQ, strerror(errno));
    close(s);
    return -1;
  }
  fd_set readfds;
  FD_ZERO(&readfds);
  int sockets[LISTENQ];
  for (int i = 0; i < LISTENQ; i++) {
    sockets[i] = -1;
  }
  for (;;) {
    FD_ZERO(&readfds);
    FD_SET(s, &readfds);
    int maxfdp1 = s + 1;
    for (int i = 0; i < LISTENQ; i++) {
      int client_s = sockets[i];
      if (client_s != -1) {
        FD_SET(client_s, &readfds);
        if (maxfdp1 < client_s + 1) {
          maxfdp1 = client_s + 1;
        }
      }
    }
    int select_result = select(maxfdp1, &readfds, nullptr, nullptr, nullptr);
    if (select_result < 0) {
      printf("select failed: %s\n", strerror(errno));
      break;
    }
    if (FD_ISSET(s, &readfds)) {
      anet::SockAddr client_addr;
      memset(&client_addr, 0, sizeof(client_addr));
      client_addr.family = AF_INET;
      socklen_t client_socklen = sizeof(client_addr.sa.v4);
      int connected_socket =
          accept(s, (struct sockaddr *)&client_addr.sa.v4, &client_socklen);
      if (connected_socket < 0) {
        printf("accept failed: %s\n", strerror(errno));
        continue;
      }
      char client_addr_buffer[100] = {0};
      anet::SockAddrPrint(client_addr_buffer, sizeof(client_addr_buffer),
                          &client_addr, true);
      printf("client: %s\n", client_addr_buffer);
      for (int i = 0; i < LISTENQ; i++) {
        if (sockets[i] == -1) {
          sockets[i] = connected_socket;
          break;
        }
      }
    }
    for (int i = 0; i < LISTENQ; i++) {
      int client_s = sockets[i];
      if (client_s == -1) {
        continue;
      }
      if (FD_ISSET(client_s, &readfds)) {
        char buffer[1024] = {0};
        int nread = recv(client_s, buffer, sizeof(buffer), 0);
        if (nread < 0) {
          printf("recv failed: %s\n", strerror(errno));
          sockets[i] = -1;
          close(client_s);
        } else if (nread == 0) {
          printf("peer closed\n");
          sockets[i] = -1;
          close(client_s);
        } else {
          printf("recv from client: %.*s\n", nread, buffer);
          int nwrite = send(client_s, buffer, nread, 0);
          if (nwrite <= 0) {
            printf("send failed: %s\n", strerror(errno));
            sockets[i] = -1;
            close(client_s);
          } else {
            printf("send to client: %.*s\n", nwrite, buffer);
          }
        }
      }
    }
  }

  close(s);
  return 0;
}
