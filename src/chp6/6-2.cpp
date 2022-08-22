/**
 * @file 6-2.cpp
 * @author balance (2570682750@qq.com)
 * @brief Web服务器上的集中写
 * @version 0.1
 * @date 2022-08-22
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
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

static const char *status_line[2] = {"200 OK", "500 Internal server error"};

int main(int argc, char *argv[]) {
  if (argc <= 3) {
    printf("usage: %s ip_address port_number backlo\n", basename(argv[0]));
    return 1;
  }
  //目标文件
  const char *file_name = argv[3];

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
    //用于保存http应答的状态行
    char header_buf[BUFFER_SIZE];
    memset(header_buf, '\0', sizeof(header_buf));
    //用于存放目标文件内容的应用程序缓存
    char *file_buf;
    //用于获取目标文件的属性，目录、文件大小
    struct stat file_stat;
    //记录目标文件是否是有效文件
    bool valid = true;
    //缓存区header_buf目前已经使用了多少字节的空间
    int len;

    if (stat(file_name, &file_stat) < 0) {  //目标文件不存在
      valid = false;
    } else {
      if (S_ISDIR(file_stat.st_mode)) {  //目标文件是一个目录
        valid = false;
      } else if (file_stat.st_mode & S_IROTH) {  //当前用户有读取目标文件的权限
        int fd = open(file_name, O_RDONLY);
        file_buf = new char[file_stat.st_size + 1];
        memset(&file_buf, '\0', file_stat.st_size + 1);
        if (read(fd, file_buf, file_stat.st_size) < 0) {
          valid = false;
        }
      } else {
        valid = false;
      }
    }
    if (valid) {
      //将HTTP应答的状态行、"Content-Length"头部字段和一个空行依次加入header_buf中
      ret = snprintf(header_buf, BUFFER_SIZE - 1, "%s %s\r\n", "HTTP/1.1",
                     status_line[0]);
      len += ret;
      ret = snprintf(header_buf + len, BUFFER_SIZE - 1 - len,
                     "Content-Length: %ld\r\n", file_stat.st_size);
      len += ret;
      ret = snprintf(header_buf + len, BUFFER_SIZE - 1 - len, "%s", "\r\n");
      struct iovec iv[2];
      iv[0].iov_base = header_buf;
      iv[0].iov_len = strlen(header_buf);
      iv[1].iov_base = file_buf;
      iv[1].iov_len = file_stat.st_size;
      ret = writev(connfd, iv, 2);
    } else {
      ret = snprintf(header_buf, BUFFER_SIZE - 1, "%s %s\r\n", "HTTP/1.1",
                     status_line[1]);
      len += ret;
      ret = snprintf(header_buf + len, BUFFER_SIZE - 1 - len, "%s", "\r\n");
      send(connfd, header_buf, strlen(header_buf), 0);
    }
    close(connfd);
    delete[] file_buf;
  }
  close(sock);
  return 0;
}