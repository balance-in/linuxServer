/**
 * @file 5-12.cpp
 * @author balance (2570682750@qq.com)
 * @brief 访问daytime服务
 * @version 0.1
 * @date 2022-08-22
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <arpa/inet.h>
#include <assert.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  assert(argc == 2);
  char *host = argv[1];
  //获取目标主机地址信息
  struct hostent *hostinfo = gethostbyname(host);
  assert(hostinfo);
  //获取daytime服务信息
  struct servent *servinfo = getservbyname("daytime", "tcp");
  assert(servinfo);
  printf("daytime port is %d\n", ntohs(servinfo->s_port));

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = servinfo->s_port;
  addr.sin_addr = *(struct in_addr *)*hostinfo->h_addr_list;

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  int result = connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));
  assert(result != -1);

  char buffer[128];
  result = read(sockfd, buffer, sizeof(buffer));
  assert(result > 0);
  buffer[result] = '\0';
  printf("the day time is: %s", buffer);
  close(sockfd);
  return 0;
}
