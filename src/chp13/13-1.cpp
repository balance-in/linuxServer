/**
 * @file 13-1.cpp
 * @author balance (2570682750@qq.com)
 * @brief SIGCHILD信号的典型处理函数
 * @version 0.1
 * @date 2022-09-15
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>

static void handle_child(int sig) {
  pid_t pid;
  int stat;
  while ((pid = waitpid(pid, &stat, WNOHANG)) > 0) {
    /*对结束的子进程进行善后处理*/
  }
}