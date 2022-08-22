/**
 * @file 5-10.cpp
 * @author balance (2570682750@qq.com)
 * @brief 修改TCP发送缓冲区的客户端程序
 * @version 0.1
 * @date 2022-08-22
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 512

int main(int argc, char *argv[]) {
  if (argc <= 2) {
    printf("usage: %s ip_address port_number backlo\n", basename(argv[0]));
    return 1;
  }

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(argv[1]);
  server_addr.sin_port = htons(atoi(argv[2]));

  int sockfd = socket(PF_INET, SOCK_STREAM, 0);
  assert(sockfd >= 0);

  int sendbuf = atoi(argv[3]);
  int len = sizeof(sendbuf);

  setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sendbuf, sizeof(sendbuf));
  getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sendbuf, (socklen_t *)&len);
  printf("the tcp send buffer size after setting is %d\n", sendbuf);

  if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) !=
      -1) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 'a', BUFFER_SIZE);
    send(sockfd, buffer, BUFFER_SIZE, 0);
  }
  

  close(sockfd);
  return 0;
}