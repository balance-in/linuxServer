/**
 * @file 6-4.cpp
 * @author balance (2570682750@qq.com)
 * @brief 使用splice函数实现的回射服务器
 * @version 0.1
 * @date 2022-08-24
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
#include <sys/uio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc <= 2) {
    printf("usage: %s ip_address port_number backlo\n", basename(argv[0]));
    return 1;
  }
  //目标文件

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
    int pipefd[2];
    assert(ret != -1);
    ret = pipe(pipefd);
    ret = splice(connfd, NULL, pipefd[1], NULL, 32768,
                 SPLICE_F_MORE | SPLICE_F_MOVE);
    assert(ret != -1);
    ret = splice(pipefd[0], NULL, connfd, NULL, 32768,
                 SPLICE_F_MORE | SPLICE_F_MOVE);
    assert(ret != -1);
    close(connfd);
  }
  close(sock);
  return 0;
}