/**
 * @file 6-1.cpp
 * @author balance (2570682750@qq.com)
 * @brief CGI服务器原理
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
  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(argv[1]);
  addr.sin_port = htons(atoi(argv[2]));

  int sock = socket(PF_INET, SOCK_STREAM, 0);
  assert(sock != -1);
  int ret = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
  assert(ret != -1);
  ret = listen(sock, 5);
  assert(ret != -1);

  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  int connfd = accept(sock, (struct sockaddr *)&client_addr, &client_addr_len);
  if (connfd < 0) {
    printf("errno is: %d\n", errno);
  } else {
    close(STDOUT_FILENO);  //关闭标准输出文件描述符(其值为1)
    dup(connfd);  // dup总是返回系统中最小的可用文件描述符，所以为1
    printf("abc\n");  // 输出到客户端而不是屏幕终端
    close(connfd);
  }

  close(sock);
  return 0;
}