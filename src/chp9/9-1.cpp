/**
 * @file 9-1.cpp
 * @author balance (2570682750@qq.com)
 * @brief 同时接收普通数据和带外数据
 * @version 0.1
 * @date 2022-09-09
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc <= 2) {
    printf("usage:%s ip_address port_number\n", basename(argv[0]));
    return 1;
  }

  struct sockaddr_in address;
  bzero(&address, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr(argv[1]);
  address.sin_port = htons(atoi(argv[2]));

  int listenfd = socket(PF_INET, SOCK_STREAM, 0);
  assert(listenfd != 0);
  int ret = bind(listenfd, (struct sockaddr *)&address, sizeof(address));
  assert(ret != -1);
  ret = listen(listenfd, 5);
  assert(ret != -1);

  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  int connfd =
      accept(listenfd, (struct sockaddr *)&client_addr, &client_addr_len);
  if (connfd < 0) {
    printf("errno is:%d\n", errno);
    close(listenfd);
  }
  char buf[1024];
  fd_set read_fds;
  fd_set exception_fds;
  FD_ZERO(&read_fds);
  FD_ZERO(&exception_fds);
  while (true) {
    memset(buf, '\0', sizeof(buf));
    /*每次调用select前都要重新在read_fds和exception_fds中设置文件描述符
    connfd，因为事件发生之后，文件描述符集合将被内核修改*/
    FD_SET(connfd, &read_fds);
    FD_SET(connfd, &exception_fds);
    ret = select(connfd + 1, &read_fds, NULL, &exception_fds, NULL);
    if (ret < 0) {
      printf("selection failure\n");
      break;
    }
    /*对于可读事件，采用普通的recv函数读取数据*/
    if (FD_ISSET(connfd, &read_fds)) {
      ret = recv(connfd, buf, sizeof(buf) - 1, 0);
      if (ret <= 0) {
        break;
      }
      printf("get%d bytes of normal data:%s\n", ret, buf);
    }
    /*对于异常事件，采用带MSG_OOB标志的recv函数读取带外数据*/
    else if (FD_ISSET(connfd, &exception_fds)) {
      ret = recv(connfd, buf, sizeof(buf), MSG_OOB);
      if (ret <= 0) break;
      printf("get%d bytes of oob data:%s\n", ret, buf);
    }
  }
  close(connfd);
  close(listenfd);
  return 0;
}
