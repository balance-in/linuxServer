/**
 * @file 10-2.cpp
 * @author balance (2570682750@qq.com)
 * @brief 使用SIGURG检测带外数据是否到达
 * @version 0.1
 * @date 2022-09-14
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
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define BUF_SIZE 1024

static int connfd;
/*SIGURG信号的处理函数*/
void sig_urg(int sig) {
  int save_errno = errno;
  char buffer[BUF_SIZE];
  memset(buffer, '\0', BUF_SIZE);
  int ret = recv(connfd, buffer, BUF_SIZE - 1, MSG_OOB); /*接收带外数据*/
  printf("got%d bytes of oob data'%s'\n", ret, buffer);
  errno = save_errno;
}
void addsig(int sig, void (*sig_handler)(int)) {
  struct sigaction sa;
  memset(&sa, '\0', sizeof(sa));
  sa.sa_handler = sig_handler;
  sa.sa_flags |= SA_RESTART;
  sigfillset(&sa.sa_mask);
  assert(sigaction(sig, &sa, NULL) != -1);
}

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("usage:%s ip_address port_number\n", basename(argv[0]));
    return 1;
  }

  struct sockaddr_in address;
  bzero(&address, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(INADDR_ANY);
  address.sin_port = htons(atoi(argv[1]));

  int listenfd = socket(PF_INET, SOCK_STREAM, 0);
  assert(listenfd != 0);
  int ret = bind(listenfd, (struct sockaddr *)&address, sizeof(address));
  if (ret == -1) {
    printf("errno is%d\n", errno);
    return 1;
  }
  ret = listen(listenfd, 5);
  assert(ret != -1);

  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  connfd = accept(listenfd, (struct sockaddr *)&client_addr, &client_addr_len);
  if (connfd < 0) {
    printf("errno is: %d\n", errno);
  } else {
    addsig(SIGURG, sig_urg);
    /*使用SIGURG信号之前，我们必须设置socket的宿主进程或进程组*/
    fcntl(connfd, F_SETOWN, getpid());
    char buffer[BUF_SIZE];
    while (true) { /*循环接收普通数据*/
      memset(buffer, '\0', BUF_SIZE);
      ret = recv(connfd, buffer, BUF_SIZE - 1, 0);
      if (ret <= 0) {
        break;
      }
      printf("got %d bytes of normal data'%s'", ret, buffer);
    }
    close(connfd);
  }
  close(listenfd);
  return 0;
}
