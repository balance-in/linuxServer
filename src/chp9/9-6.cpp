/**
 * @file 9-6.cpp
 * @author balance (2570682750@qq.com)
 * @brief 聊天室客户端程序
 * @version 0.1
 * @date 2022-09-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#define _GNU_SOURCE 1
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
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 64
int main(int argc, char* argv[]) {
  if (argc <= 2) {
    printf("usage:%s ip_address port_number\n", basename(argv[0]));
    return 1;
  }
  struct sockaddr_in addr;
  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(argv[1]);
  addr.sin_port = htons(atoi(argv[2]));

  int sockfd = socket(PF_INET, SOCK_STREAM, 0);
  assert(sockfd >= 0);
  int ret = connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
  if (ret < 0) {
    printf("connection failed\n");
    close(sockfd);
    return 1;
  }
  pollfd fds[2];
  /*注册文件描述符0（标准输入）和文件描述符sockfd上的可读事件*/
  fds[0].fd = 0;
  fds[0].events = POLLIN;
  fds[0].revents = 0;
  fds[1].fd = sockfd;
  fds[1].events = POLLIN | POLLRDHUP;
  fds[1].revents = 0;
  char read_buf[BUFFER_SIZE];
  int pipefd[2];
  ret = pipe(pipefd);
  assert(ret != -1);
  while (true) {
    ret = poll(fds, 2, -1);
    if (ret < 0) {
      printf("poll failure\n");
      break;
    }
    if (fds[1].revents & POLLRDHUP) {
      printf("server close the connection\n");
      break;
    } else if (fds[1].revents & POLLIN) {
      memset(read_buf, '\0', BUFFER_SIZE);
      recv(fds[1].fd, read_buf, BUFFER_SIZE - 1, 0);
      printf("%s\n", read_buf);
    }
    if (fds[0].revents & POLLIN) {
      /*使用splice将用户输入的数据直接写到sockfd上（零拷贝）*/
      ret = splice(0, NULL, pipefd[1], NULL, 32768,
                   SPLICE_F_MORE | SPLICE_F_MOVE);
      ret = splice(pipefd[0], NULL, sockfd, NULL, 32768,
                   SPLICE_F_MORE | SPLICE_F_MOVE);
    }
  }
  close(sockfd);
  return 0;
}
