# 网络编程

## 1 基本数据结构

### 1.1 sockaddr

网络地址通用的数据结构为`struct sockaddr`, 接口中参数类型一般为`struct sockaddr *`, 如果是`IPv4`, 参数实际的数据结构为`struct sockaddr_in`, 如果是`IPv6`, 实际的数据结构应该为`struct sockaddr_in6`.

我们可以把`struct sockaddr`看作是网络地址的**基类**，而`struct sockaddr_in`和`struct sockaddr_in6`作为**子类**。

```c
// 通用
struct sockaddr {
  uint16_t sin_family;
  char sa_data[14];
};

// IPv4
typedef uint32_t in_addr_t;
struct in_addr {
  in_addr_t s_addr;
};
struct sockaddr_in {
  int16_t sin_family;
  uint16_t sin_port;
  struct in_addr sin_addr;
  char sin_zero[8];
};

// IPv6
struct sockaddr_in6 {
  int16_t sin6_family;
  uint16_t sin6_port;
  uint32_t sin6_flowinfo;
  uint8_t sin6_addr[16];
  uint32_t sin6_scope_id;
};
```

### 1.2 addrinfo

`struct addrinfo`用于域名解析使用，通常服务器的地址一般是可读的域名，如'www.bing.com'， 'www.baidu.com'等。我们需要将域名解析为网络地址。这里我们就需要使用`getaddrinfo`和`freeaddrinfo`接口。

```c
struct addrinfo {
  int ai_flags; // AI_PASSIVE, AI_CANONNAME, AI_NUMERICHOST, ...
  int ai_family; // AF_INET, AF_INET6
  int ai_socktype; // SOCK_STREAM, SOCK_DGRAM
  int ai_protocol; // 通常使用0，表示任意
  socklen_t ai_addrlen; // ai_addr的字节长度
  struct sockaddr *ai_addr; // struct sockaddr_in , struct sockaddr_in6
  char *ai_canonname;
  struct addrinfo *ai_next;
};
```

接着看一下解析的接口

```c
/**
 * 域名解析
 * 域名地址和服务名不能同时为null
 *
 * @param hostname 域名地址
 * @param servname 端口或对应的服务名
 * @param hints 解析选项
 * @param res 结果，需要使用freeaddrinfo释放
 * @return 0表示成功, 非0表示失败，可通过errno和gai_strerror()查看对应错误码和描述
 */
int getaddrinfo(const char *hostname, const char *servname, const struct addrinfo *hints, struct addrinfo **res);

// 常用的使用方式
struct addrinfo hints;
struct addrinfo *res;
// 配置解析选项
memset(&hints, 0, sizeof(hints));
hints.ai_family = PF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;
// 需要判断返回值
getaddrinfo(hostname, servname, &hints, &res);
for (struct addrinfo *p = res; p; p = p->ai_next) {
  // 创建socket
  s = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
  if (s < 0) {
    continue;
  }
  // 连接
  connect_ok = connect(s, p->ai_addr, p->ai_addrlen);
  if (connect_ok) {
    break;
  } else {
    close(s);
    continue;
  }
}
if (s < 0) {
  // Error: 没有合适的地址
}
freeaddrinfo(res);
```

### 1.3 常量

一般我们会使用一些常量，比如`socket()`方法，需要3个参数分别是`family`, `socktype`和`protocol`。不同的参数表示创建不同类型的`socket`.

```c
// 一般我们用address family多一些，基本上protocol family与address family一致。
// address family
enum ADDRESS_FAMILY {
  AF_UNIX,
  AF_LOCAL,
  AF_INET,
  AF_INET6,
  AF_UNSPEC,
};
// protocol family
enum PROTOCOL_FAMILY {
  PF_UNIX,
  PF_LOCAL,
  PF_INET,
  PF_INET6,
  PF_UNSPEC,
};

// socktype
enum SOCKTYPE {
  SOCK_STREAM,
  SOCK_DGRAM,
  SOCK_RAW,
};

// protocol
// protocol一般我们都传0
```

## 2 常用接口

### 2.1 创建socket

```c
/**
 * 创建socket
 *
 * @param family 常用的有AF_INET, AF_INET6
 * @param socktype 类型，常用的有SOCK_STREAM, SOCK_DGRAM
 * @param protocol 通常传0即可
 * @return 大于0表示成功，返回对应的socket，小于0说明失败了
 */
int socket(int family, int socktype, int protocol);
```

### 2.2 连接

```c
/**
 * 连接, 普通socket会变成已连接的socket
 *
 * @param socket 普通socket
 * @param sa 服务器地址
 * @param sock_len 服务器地址结构的长度
 * @return 如果成功返回0，否则返回小于0的数(一般为-1)
 */
int connect(int socket, const struct sockaddr *sa, socklen_t sock_len);
```

### 2.3 绑定

```c
/**
 * 将socket绑定特定的地址
 *
 * @param socket 普通socket
 * @param sa 地址
 * @param sock_len 地址类型结构的长度
 * @return 如果成功返回0, 否则返回小于0的数(一般为-1)
 */
int bind(int socket, const struct sockaddr *sa, socklen_t sock_len);
```

### 2.4 监听

```c
/**
 *将普通socket变成监听socket
 *
 * @param socket 普通socket
 * @param listenq 监听的最大数量
 * @return 如果成功返回0，否则返回小于0的数(一般为-1)
 */
int listen(int socket, int listenq);
```

### 2.5 Accept

```c
/**
 * 有新的连接到来
 *
 * @param socket 监听socket
 * @param sa 传出参数，对端的地址, 可以传NULL忽略
 * @param sock_len 地址类型结构的长度，可以传NULL忽略, 如果不为NULL，初始值必须是sizeof(sockaddr_in) 或 sizeof(sockaddr_in6)
 * @return 已连接的socket
 */
int accept(int socket, struct sockaddr *sa, socklen_t *sock_len);
```

### 2.6 发送数据

`socket` 可以作为一种文件描述符，可看作文件处理，我们可以直接使用系统调用`write`发送和`read`读取数据。
其中`send`和`write`都可以发送数据，如果`send`方法的flags为0，则与`write`相同的功能。`send`和`write`中的`socket`参数都必须是已连接的`socket`, 否则会发送数据失败.

#### 2.6.1 write

```c
/**
 * 发送数据
 * @param fd 已连接的socket
 * @param buf 数据
 * @param size 数据大小
 * @return 发送数据的大小，如果小于0则说明发送失败
 */
int write(int fd, const void *buf, size_t size);
```

#### 2.6.2 send

```c
// flags一般传0
int send(int socket, const void *buf, size_t size, int flags);
```

#### 2.6.3 sendmsg

```c
int sendmsg(int socket, const struct msghdr *message, int flags);
```

#### 2.6.4 sendto

```c
// 一般用于udp
int sendto(int socket, const void *buf, size_t size, int flags, const struct sockaddr *sa, socklen_t sock_len);
```

### 2.7 接收数据

其中`recv`和`read`都可以接收数据，如果`recv`方法的flags为0，则与`read`相同的功能。`recv`和`read`中的`socket`参数都必须是已连接的`socket`, 否则会接收数据失败.

#### 2.7.1 read

```c
/**
 * 接收数据
 *
 * @param socket 已连接的socket
 * @param buf 接收数据的缓存
 * @param size 缓存大小
 * @return 接收数据大小, 如果为0，表示对端已关闭，如果小于0说明接收数据出错
 */
int read(int socket, void *buf, size_t size);
```

#### 2.7.2 recv

```c
// flags一般传0
int recv(int socket, void *buf, size_t size, int flags);
```

#### 2.7.3 recvmsg

```c
int recvmsg(int socket, struct msghdr *message, int flags);
```

#### 2.7.4 recvfrom

```c
// 一般用于udp
int recvfrom(int socket, void *buf, size_t size, int flags,
struct sockaddr *sa, socklen_t *sock_len);
```

### 2.8 关闭

```c
/**
 * 关闭socket，可根据how决定关闭发送端和接收端
 *
 * @param socket 已连接的socket
 * @param how SHUT_RD表示接收端；SHUT_WR表示发送端；SHUT_RDWR表示接收端和发送端
 * @return 0表示成功，小于0表示失败
 */
int shutdown(int socket, int how);
```

### 2.9 销毁

```c
/**
 * 销毁socket，会同时关闭发送端和接收端
 * @param fd 文件描述符
 * @return 0表示成功，小于0表示失败
 */
int close(int fd);
```

### 2.10 网络地址转换为字符串

这里我们只讨论`inet_ntop`接口，该接口可同时支持`IPv4`和`IPv6`。

```c
/**
 * 将网络地址转换为字符串
 *
 * @param family AF_INET表示IPv4; AF_INET6表示IPv6
 * @param src 网络地址，如果IPv4，src应该指向sockaddr_in.sin_addr; 如果IPv6, src应该指向sockaddr_in6.sin6_addr
 * @param dst 字符串缓存，如果IPv4，字符串大小至少为INET_ADDRSTRLEN(16); 如果IPv6，至少为INET6_ADDRSTRLEN(46)
 * @param socklen 网络地址结构长度，如果IPv4，应该为sizeof(sockaddr_in)；如果IPv6，应该为sizeof(sockaddr_in6)
 * @return 字符串，NULL表示转换出错
 */
const char *inet_ntop(int family, const void *src, char *dst, socklen_t socklen);
```

### 2.11 字符串转换为网络地址

```c
/**
 * 将字符串转换为网络地址
 *
 * @param family AF_INET表示IPv4; AF_INET6表示IPv6
 * @param src 地址字符串
 * @param dst 网络地址，如果IPv4，dst应该指向sockaddr_in.sin_addr; 如果IPv6, dst应该指向sockaddr_in6.sin6_addr
 * @return 大于0表示成功，等于0表示地址与family冲突，小于0表示转换出错
 */
int inet_pton(int family, const char *src, void *dst);
```
