/**
 * @file 5-6.cpp
 * @author balance (2570682750@qq.com)
 * @brief 发送带外数据
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

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(argv[1]);
  server_addr.sin_port = htons(atoi(argv[2]));

  int sockfd = socket(PF_INET, SOCK_STREAM, 0);
  assert(sockfd >= 0);

  if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    printf("connection failed\n");
  } else {
    const char *oob_data = "abc";
    const char *normal_data = "123";
    send(sockfd, normal_data, strlen(normal_data), 0);
    send(sockfd, oob_data, strlen(oob_data), MSG_OOB);
    send(sockfd, normal_data, strlen(normal_data), 0);
  }

  close(sockfd);
  return 0;
}