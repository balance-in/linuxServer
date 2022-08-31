/**
 * @file 7-3.cpp
 * @author balance (2570682750@qq.com)
 * @brief 服务器程序以守护进程的方式运行
 * @version 0.1
 * @date 2022-08-31
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

bool daemonsize() {
  pid_t pid = fork();
  if (pid < 0) {
    return false;
  } else if (pid > 0) {
    exit(0);
  }
  //设置文件掩码。进程创建文件的权限将是mode&0777
  umask(0);
  //创建新的会话，设置本进程为进程组的首领
  pid_t sid = setsid();
  if (sid < 0) {
    return false;
  }
  //切换工作目录
  if ((chdir("/")) < 0) {
    return false;
  }
  //关闭标准输入设备，标准输出设备和标准错误输出设备
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
  //将标准输入、标准输出和标准错误都重定向到/dev/null文件
  open("/dev/null", O_RDONLY);
  open("/dev/null", O_RDWR);
  open("/dev/null", O_RDWR);
  return true;
}