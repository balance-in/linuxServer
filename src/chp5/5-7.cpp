/**
 * @file 5-7.cpp
 * @author balance (2570682750@qq.com)
 * @brief 接收带外数据
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
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
  if (argc <= 2) {
    printf("usage: %s ip_address port_number backlo\n", basename(argv[0]));
    return 1;
  }
  const char *ip = argv[1];
  int port = atoi(argv[2]);

  int sock = socket(PF_INET, SOCK_STREAM, 0);
  assert(sock >= 0);

  struct sockaddr_in address;
  bzero(&address, sizeof(address));
  address.sin_family = AF_INET;
  inet_pton(AF_INET, ip, &address.sin_addr);
  address.sin_port = htons(port);

  int ret = bind(sock, (struct sockaddr *)&address, sizeof(address));
  assert(ret != -1);

  ret = listen(sock, 5);
  assert(ret != -1);
  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  int connfd = accept(sock, (struct sockaddr *)&client_addr, &client_addr_len);
  if (connfd < 0) {
    printf("errno is: %d\n", errno);
  } else {
    char buffer[BUF_SIZE];

    memset(buffer, '\0', BUF_SIZE);
    ret = recv(connfd, buffer, BUF_SIZE - 1, 0);
    printf("got %d bytes of normal data '%s'\n", ret, buffer);

    memset(buffer, '\0', BUF_SIZE);
    ret = recv(connfd, buffer, BUF_SIZE - 1, MSG_OOB);
    printf("got %d bytes of normal data '%s'\n", ret, buffer);

    memset(buffer, '\0', BUF_SIZE);
    ret = recv(connfd, buffer, BUF_SIZE - 1, 0);
    printf("got %d bytes of normal data '%s'\n", ret, buffer);

    close(connfd);
  }

  close(sock);
  return 0;
}