/**
 * @file 15-2.cpp
 * @author balance (2570682750@qq.com)
 * @brief 用进程池实现的并发CGI服务器
 * @version 0.1
 * @date 2022-09-22
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "processpool.h" /*进程池*/

/*用于处理客户CGI请求的类，它可以作为processpool类的模板参数*/
class cgi_conn {
 public:
  cgi_conn() {}
  ~cgi_conn() {}
  /*初始化客户连接，清空读缓冲区*/
  void init(int epollfd, int sockfd, const sockaddr_in& client_addr) {
    m_epollfd = epollfd;
    m_sockfd = sockfd;
    m_address = client_addr;
    memset(&m_address, '\0', BUFFER_SIZE);
    m_read_idx = 0;
  }
  void process() {
    int idx = 0;
    int ret = -1;
    /*循环读取和分析客户数据*/
    while (true) {
      idx = m_read_idx;
      ret = recv(m_sockfd, m_buf + idx, BUFFER_SIZE - 1 - idx, 0);
      /*如果读操作发生错误,则关闭客户连接。但如果是暂时无数据可读，则退出循环*/
      if (ret < 0) {
        if (errno != EAGAIN) {
          removefd(m_epollfd, m_sockfd);
        }
        break;
      }
      /*如果对方关闭连接，则服务器也关闭连接*/
      else if (ret == 0) {
        removefd(m_epollfd, m_sockfd);
        break;
      } else {
        m_read_idx += ret;
        printf("user content is : %s\n", m_buf);
        /*如果遇到字符“\r\n”，则开始处理客户请求*/
        for (; idx < m_read_idx; ++idx) {
          if ((idx >= 1) && (m_buf[idx - 1] == '\r') && (m_buf[idx] == '\n')) {
            break;
          }
        }
        /*如果没有遇到字符“\r\n”，则需要读取更多客户数据*/
        if (idx == m_read_idx) {
          continue;
        }
        m_buf[idx - 1] = '\0';
        char* file_name = m_buf;
        /*判断客户要运行的CGI程序是否存在*/
        if (access(file_name, F_OK) == -1) {
          removefd(m_epollfd, m_sockfd);
          break;
        }
        /*创建子进程来执行CGI程序*/
        ret = fork();
        if (ret == -1) {
          removefd(m_epollfd, m_sockfd);
          break;
        } else if (ret > 0) {
          /*父进程只需关闭连接*/
          removefd(m_epollfd, m_sockfd);
          break;
        } else {
          /*子进程将标准输出定向到m_sockfd，并执行CGI程序*/
          close(STDOUT_FILENO);
          dup(m_sockfd);
          execl(m_buf, m_buf, 0);
          exit(0);
        }
      }
    }
  }

 private:
  /*读缓冲区的大小*/
  static const int BUFFER_SIZE = 1024;
  static int m_epollfd;
  int m_sockfd;
  sockaddr_in m_address;
  char m_buf[BUFFER_SIZE];
  /*标记读缓冲中已经读入的客户数据的最后一个字节的下一个位置*/
  int m_read_idx;
};
int cgi_conn::m_epollfd = -1;

int main(int argc, char* argv[]) {
  if (argc <= 1) {
    printf("usage:%s ip_address port_number\n", basename(argv[0]));
    return 1;
  }

  struct sockaddr_in address;
  bzero(&address, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(INADDR_ANY);
  address.sin_port = htons(atoi(argv[1]));

  int listenfd = socket(PF_INET, SOCK_STREAM, 0);
  assert(listenfd != 0);
  int ret = bind(listenfd, (struct sockaddr*)&address, sizeof(address));
  if (ret == -1) {
    printf("errno is%d\n", errno);
    return 1;
  }
  ret = listen(listenfd, 5);
  assert(ret != -1);

  processpool<cgi_conn>* pool = processpool<cgi_conn>::create(listenfd);
  if (pool) {
    pool->run();
    delete pool;
  }
  close(listenfd);
  return 0;
}