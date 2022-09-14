/**
 * @file 9-5.cpp
 * @author balance (2570682750@qq.com)
 * @brief 非阻塞connect
 * @version 0.1
 * @date 2022-09-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 1023
int setnonblocking(int fd) {
  int old_option = fcntl(fd, F_GETFL);
  int new_option = old_option | O_NONBLOCK;
  fcntl(fd, F_SETFL, new_option);
  return old_option;
}
/*超时连接函数，参数分别是服务器IP地址、端口号和超时时间（毫秒）。函数成功时
返回已经处于连接状态的socket，失败则返回-1*/
int unblock_connect(const char *ip, int port, int time) {
  int ret = 0;
  struct sockaddr_in addr;
  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(ip);
  addr.sin_port = htons(port);

  int sockfd = socket(PF_INET, SOCK_STREAM, 0);
  int fdopt = setnonblocking(sockfd);
  ret = connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));
  if (ret == 0) {
    /*如果连接成功，则恢复sockfd的属性，并立即返回之*/
    printf("connect with server immdeiately\n");
    fcntl(sockfd, F_SETFL, fdopt);
    return sockfd;
  } else if (errno != EINPROGRESS) {
    /*如果连接没有立即建立，那么只有当errno是EINPROGRESS时才表示连接还在进
    行，否则出错返回*/
    printf("unblock connect not support\n");
    return -1;
  }
  fd_set readfds;
  fd_set writefds;
  struct timeval timeout;
  FD_ZERO(&readfds);
  FD_SET(sockfd, &writefds);
  timeout.tv_sec = time;
  timeout.tv_usec = 0;
  ret = select(sockfd + 1, NULL, &writefds, NULL, &timeout);
  if (ret <= 0) {
    /*select超时或者出错，立即返回*/
    printf("connection time out\n");
    close(sockfd);
    return -1;
  }
  if (!FD_ISSET(sockfd, &writefds)) {
    printf("no events on sockfd found\n");
    close(sockfd);
    return -1;
  }
  int error = 0;
  socklen_t len = sizeof(error);
  /*调用getsockopt来获取并清除sockfd上的错误*/
  if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
    printf("get socket option failed\n");
    close(sockfd);
    return -1;
  }
  /*错误号不为0表示连接出错*/
  if (error != 0) {
    printf("connection faile after select with the error:%d", error);
    close(sockfd);
    return -1;
  }
  /*连接成功*/
  printf("connection ready after select with the socket:%d\n", sockfd);
  fcntl(sockfd, F_SETFL, fdopt);
  return sockfd;
}

int main(int argc, char *argv[]) {
  if (argc <= 2) {
    printf("usage:%s ip_address port_number\n", basename(argv[0]));
    return 1;
  }
  const char *ip = argv[1];
  int port = atoi(argv[2]);
  int sockfd = unblock_connect(ip, port, 10);
  if (sockfd < 0) {
    return 1;
  }
  close(sockfd);
  return 0;
}