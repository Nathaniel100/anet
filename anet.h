#ifndef ANET_H
#define ANET_H

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif

#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 46
#endif

namespace anet {

// 网络地址, 支持IPv4和IPv6
struct SockAddr {
  union {
    struct sockaddr
        sa;  // 通用的网络地址结构, 一般用于connect，accept等接口参数
    struct sockaddr_in v4;   // IPv4网络地址结构
    struct sockaddr_in6 v6;  // IPv6网络地址结构
  } sa;
  int family;  // IPv4为AF_INET, IPv6为AF_INET6
};

/**
 * 获取网络地址数据结构的长度
 * 如果IPv4，则为sizeof(v4); 如果IPv6，则为sizeof(v6); 其他为sizeof(sa)
 *
 * @param sa 网络地址
 * @return 网络地址数据结构长度
 */
int SockAddrLen(const SockAddr *sa);

/**
 * 设置ipv4地址和端口
 * 注：该方法不做dns解析
 *
 * @param sa 待设置的网络地址
 * @param host ipv4地址, 如果为null，则地址为0.0.0.0
 * @param port 端口
 * @return 0表示成功，-1表示失败
 */
int SockAddrSetV4(SockAddr *sa, const char *host, uint16_t port);

/**
 * 设置IPv6地址和端口
 * 注：该方法不做DNS解析
 *
 * @param sa 待设置的网络地址
 * @param host ipv6地址, 如果为null，则地址为::0
 * @param port 端口
 * @return 0表示成功，-1表示失败
 */
int SockAddrSetV6(SockAddr *sa, const char *host, uint16_t port);

/**
 * 设置地址和端口，从addrinfo的第一个节点中设置
 *
 * @param sa 待设置的网络地址
 * @param ai 地址结构
 * @return 0表示成功，-1表示失败
 */
int SockAddrSetAddrInfo(SockAddr *sa, const struct addrinfo *ai);

/**
 * 设置端口
 *
 * @param sa 网络地址
 * @param port 端口
 * @return 0表示成功，-1表示失败
 */
int SockAddrSetPort(SockAddr *sa, uint16_t port);

/**
 * 获取端口
 * @param sa 网络地址
 * @return 端口, 0表示失败
 */
uint16_t SockAddrGetPort(const SockAddr *sa);

/**
 * 打印网络地址，将可读的网络地址写入buffer中
 * 网络地址格式: <host>[ <port>]
 *
 * @param buffer 缓存
 * @param buffer_size 缓存大小
 * @param sa 网络地址
 * @param want_port 是否打印端口
 */
int SockAddrPrint(char *buffer, uint32_t buffer_size, const SockAddr *sa,
                  bool want_port);

}  // namespace anet

#endif
