/**
 * @file 5-2.cpp
 * @author balance (2570682750@qq.com)
 * @brief backlog参数
 * @version 0.1
 * @date 2022-08-22
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <arpa/inet.h>
#include <assert.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static bool stop = false;

/* SIGTERM信号的处理函数， 触发时结束主程序中的循环*/
static void handle_term(int sig) { stop = true; }

int main(int argc, char *argv[]) {
  signal(SIGTERM, handle_term);

  if (argc <= 3) {
    printf("usage: %s ip_address port_number backlo\n", basename(argv[0]));
    return 1;
  }
  const char *ip = argv[1];
  int port = atoi(argv[2]);
  int backlog = atoi(argv[3]);

  int sock = socket(PF_INET, SOCK_STREAM, 0);
  assert(sock >= 0);

  struct sockaddr_in address;
  bzero(&address, sizeof(address));
  address.sin_family = AF_INET;
  inet_pton(AF_INET, ip, &address.sin_addr);
  address.sin_port = htons(port);

  int ret = bind(sock, (struct sockaddr *)&address, sizeof(address));
  assert(ret != -1);

  ret = listen(sock, backlog);
  assert(ret != -1);

  while (!stop) {
    sleep(1);
  }
  close(sock);
  return 0;
}