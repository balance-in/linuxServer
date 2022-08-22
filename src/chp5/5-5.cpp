/**
 * @file 5-5.cpp
 * @author balance (2570682750@qq.com)
 * @brief 接受一个异常的来凝结
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

int main(int argc, char *argv[]) {
  if (argc <= 2) {
    printf("usage: %s ip_address port_number backlo\n", basename(argv[0]));
    return 1;
  }

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(argv[1]);
  addr.sin_port = htons(atoi(argv[2]));

  int sock = socket(PF_INET, SOCK_STREAM, 0);
  assert(sock >= 0);

  int ret = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
  assert(ret != -1);

  ret = listen(sock, 5);
  assert(ret != -1);

  sleep(20);
  struct sockaddr_in client;
  socklen_t client_addr_len = sizeof(client);
  int connfd = accept(sock, (struct sockaddr *)&client, &client_addr_len);
  if (connfd < 0) {
    printf("errno is: %d\n", errno);
  } else {
    char remnote(INET_ADDRSTRLEN);
    printf("connected with ip: %s and port: %d\n", inet_ntoa(client.sin_addr),
           ntohs(client.sin_port));
    close(connfd);
  }
  close(connfd);
  return 0;
}