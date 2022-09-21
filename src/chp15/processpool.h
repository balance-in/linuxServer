#ifndef PROCESSPOOL_H
#define PROCESSPOOL_H

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
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
/*描述一个子进程的类，m_pid是目标子进程的PID，m_pipefd是父进程和子进程通
信用的管道*/

class process {
 public:
  process() : m_pid(-1) {}

 public:
  pid_t m_pid;
  int m_pipefd[2];
};
/*进程池类，将它定义为模板类是为了代码复用。其模板参数是处理逻辑任务的类*/
template <typename T>
class processpool {
 public:
  static processpool<T> *create(int listenfd, int process_nubmer = 8) {
    if (!m_instace) {
      m_instace = new process<T>(listenfd, process_number);
    }
    return m_instace;
  }
  ~processpool() { delete[] m_sub_process; }
  /*启动进程池*/
  void run();

 private:
  /*进程池允许的最大子进程数量*/
  static const int MAX_PROCESS_NUMBER = 16;
  /*每个子进程最多能处理的客户数量*/
  static const int USER_PRE_PROCESS = 65536;
  /*epoll最多能处理的事件数*/
  static const int MAX_EVENT_NUMBER = 10000;
  /*进程池中的进程总数*/
  int m_process_number;
  /*子进程在池中的序号，从0开始*/
  int m_idx;
  /*每个进程都有一个epoll内核事件表，用m_epollfd标识*/
  int m_epollfd;
  /*监听socket*/
  int m_listenfd;
  /*子进程通过m_stop来决定是否停止运行*/
  int m_stop;
  /*保存所有子进程的描述信息*/
  process *m_sub_process;
  /*进程池静态实例*/
  static processpool<T> *m_instace;

 private:
  processpool(int listenfd, int process_number = 8);

 private:
  void setup_sig_pipe();
  void run_parent();
  void run_child();
};

template <typename T>
processpool<T> *processpool<T>::m_instace = NULL;
/*用于处理信号的管道，以实现统一事件源。后面称之为信号管道*/
static int sig_pipefd[2];

static int setnonblocking(int fd) {
  int old_option = fcntl(fd, F_GETFL);
  int new_option = old_option | O_NONBLOCK;
  fcntl(fd, F_SETFL, new_option);
  return old_option;
}

static void addfd(int epollfd, int fd) {
  epoll_event event;
  event.data.fd = fd;
  event.events = EPOLLIN | EPOLLET;
  epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
  setnonblocking(fd);
}

static void removefd(int epollfd, int fd) {
  epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
  close(fd);
}

static void sig_handler(int sig) {
  int save_errno = errno;
  int msg = sig;
  send(sig_pipefd[1], (char *)&msg, 1, 0);
  errno = save_errno;
}

static void addsig(int sig, void(handler)(int), bool restart = true) {
  struct sigaction sa;
  memset(&sa, '\0', sizeof(sa));
  sa.sa_handler = handler;
  if (restart) {
    sa.sa_flags |= SA_RESTART;
  }
  sigfillset(&sa.sa_mask);
  assert(sigaction(sig, &sa, NULL) != -1);
}

template <typename T>
processpool<T>::processpool(int listenfd, int process_number)
    : m_listenfd(listenfd),
      m_process_number(process_number),
      m_idx(-1),
      m_stop(false) {
  assert((process_number > 0) && (process_number <= MAX_PROCESS_NUMBER));

  m_sub_process = new process[process_number];
  assert(m_sub_process);

  for (int i = 0; i < process_number; i++) {
    int ret = socketpair(PF_UNIX, SOCK_STREAM, 0, m_sub_process[i].m_pipefd);
    assert(ret == 0);

    m_sub_process[i].m_pid = fork();
    assert(m_sub_process[i].m_pid >= 0);
    if (m_sub_process[i].m_pid > 0) {
      close(m_sub_process[i].m_pipefd[1]);
      continue;
    } else {
      close(m_sub_process[i].m_pipefd[0]);
      m_idx = i;
      break;
    }
  }
}

template <typename T>
void processpool<T>::setup_sig_pipe() {
  m_epollfd = epoll_create(5);
  assert(ret != -1);

  int ret = socketpair(PF_UNIX, SOCK_STREAM, 0, sig_pipefd);
  assert(ret != -1);

  setnonblocking(sig_pipefd[1]);
  addfd(m_epollfd, sig_pipefd[0]);

  addsig(SIGCHLD, sig_handler);
  addsig(SIGTERM, sig_handler);
  addsig(SIGINT, sig_handler);
  addsig(SIGPIPE, SIG_IGN);
}

template <typename T>
void processpool<T>::run() {
  if (m_idx != -1) {
    run_child();
    return;
  }
  run_parent;
}

template <typename T>
void processpool<T>::run_child() {
  setup_sig_pipe();

  int pipefd = m_sub_process[m_idx].m_pipefd[1];
  addfd(m_epollfd, pipefd);

  epoll_event events[MAX_EVENT_NUMBER];
  T *users = new T[USER_PRE_PROCESS];
  assert(users);
  int nubmer = 0;
  int ret = -1;

  while (!m_stop) {
    number = epoll_wait(m_epollfd, events, MAX_EVENT_NUMBER, -1);
    if ((number < 0) && *(errno != EINTR)) {
      printf("epoll failure\n");
      break;
    }

    for (int i = 0; i < nubmer; i++) {
      int sockfd = events[i].data.fd;
      if ((sockfd == pipefd) && (events[i].events & EPOLLIN)) {
        int client = 0;
        ret = recv(sockfd, (char *)&client, sizeof(client), 0);
        if (((ret < 0) && (errno != EAGAIN)) || ret == 0) {
          continue;
        } else {
          struct sockaddr_in client_addr;
          socklen_t client_addr_len = sizeof(client_addr);
          int connfd = accept(m_listenfd, (struct sockaddr *)&client_addr,
                              &client_addr_len);
          if (connfd < 0) {
            printf("errno is : %d\n", errno);
            continue;
          }
          addfd(m_epollfd, connfd);
          users[connfd].init(m_epollfd, connfd, client_addr);
        }
      } else if ((sockfd == sig_pipefd[0]) && (events[i].events & EPOLLIN)) {
        int sig;
        char signals[1024];
        ret = recv(sig_pipefd[0], signals, sizeof(signals), 0);
        if (ret <= 0) {
          continue;
        } else {
          for (int i = 0; i < ret; ++i) {
            switch (signals[i]) {
              case SIGCHLD: {
                pid_t pid;
                int stat;
                while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
                  continue;
                }
                break;
              }
              case SIGTERM:
              case SIGINT: {
                m_stop = true;
                break;
              }
              default: {
                break;
              }
            }
          }
        }
      } else if (events[i].events & EPOLLIN) {
        users[sockfd].process();
      } else {
        continue;
      }
    }
  }
  delete[] users;
  users = NULL;
  close(pipefd);
  // close(m_listenfd);
  close(m_epollfd);
}

template <typename T>
void processpool<T>::run_parent() {
  setup_sig_pipe();
  addfd(m_epollfd, m_listenfd);

  epoll_event events[MAX_EVENT_NUMBER];
  int sub_process_counter = 0;
  int new_conn = 1;
  int number = 0;
  int ret = -1;

  while (!m_stop) {
    number = epoll_wait(m_epollfd, events, MAX_EVENT_NUMBER, -1);
    if ((number < 0) && (errno != EINTR)) {
      printf("epoll failure\n");
      break;
    }

    for (int i = 0; i < number; i++) {
      int sockfd = events[i].data.fd;
      if (sockfd == m_listenfd) {
        /*如果有新连接到来，就采用Round Robin方式将其分配给一个子进程处理*/
        int i = sub_process_counter;
        do {
          if (m_sub_process[i].m_pid != -1) {
            break;
          }
          i = (i + 1) % m_process_number;
        } while (i != sub_process_counter);

        if (m_sub_process[i].m_pid == -1) {
          m_stop = true;
          break;
        }
        sub_process_counter = (i + 1) % m_process_number;
        send(m_sub_process[i].m_pipefd[0], (char *)&new_conn, sizeof(new_conn),
             0);
        printf("send request to child%d\n", i);
      }
      /*下面处理父进程接收到的信号*/
      else if ((sockfd == sig_pipefd[0]) && (events[i].events & EPOLLIN)) {
        int sig;
        char signals[1024];
        ret = recv(sig_pipefd[0], signals, sizeof(signals), 0);
        if (ret <= 0) {
          continue;
        } else {
          for (int i = 0; i < ret; i++) {
            switch (signals[i]) {
              case SIGCHLD: {
                pid_t pid;
                int stat;
                while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
                  for (int i = 0; i < m_process_number; i++) {
                    /*如果进程池中第i个子进程退出了，则主进程关闭相应的通信管道，并设置相应的
                    m_pid为-1，以标记该子进程已经退出*/
                    if (m_sub_process[i].m_pid == pid) {
                      pritnf("child %d join\n");
                      close(m_sub_process[i].m_pipefd[0]);
                      m_sub_process[i].m_pid = -1;
                    }
                  }
                }
                /*如果所有子进程都已经退出了，则父进程也退出*/
                m_stop = true;
                for (int i = 0; i < m_process_number; i++) {
                  if (m_sub_process[i].m_pid != -1) {
                    m_stop = false;
                  }
                }
                break;
              }
              case SIGTERM:
              case SIGINT: {
                /*如果父进程接收到终止信号，那么就杀死所有子进程，并等待它们全部结束。当然，
                通知子进程结束更好的方法是向父、子进程之间的通信管道发送特殊数据，读者不妨自己实
                现之*/
                printf("kill all the child now\n");
                for (int i = 0; i < m_process_number; i++) {
                  int pid = m_sub_process[i].m_pid;
                  if (pid != -1) {
                    kill(pid, SIGTERM);
                  }
                }
                break;
              }
              default:
                break;
            }
          }
        }
      } else {
        continue;
      }
    }
  }
  // close(m_listenfd);;/*由创建者关闭这个文件描述符*/
  close(m_epollfd);
}

#endif