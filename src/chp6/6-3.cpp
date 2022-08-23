/**
 * @file 6-3.cpp
 * @author balance (2570682750@qq.com)
 * @brief 使用sendfile函数传输文件
 * @version 0.1
 * @date 2022-08-23
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
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc <= 3) {
    printf("usage: %s ip_address port_number backlo\n", basename(argv[0]));
    return 1;
  }
  //目标文件
  const char *file_name = argv[3];
  int filefd = open(file_name, O_RDONLY);
  assert(filefd > 0);
  struct stat stat_buf;
  fstat(filefd, &stat_buf);

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
    printf("errno is : %d\n", errno);
  } else {
    sendfile(connfd, filefd, NULL, stat_buf.st_size);
    close(connfd);
  }

  close(sock);
  return 0;
}