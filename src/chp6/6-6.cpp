/**
 * @file 6-6.cpp
 * @author balance (2570682750@qq.com)
 * @brief 将文件描述符设置为非阻塞的
 * @version 0.1
 * @date 2022-08-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <fcntl.h>

int setnonblocking(int fd) {
  int old_option = fcntl(fd, F_GETFL);
  int new_option = old_option | O_NONBLOCK;
  fcntl(fd, F_SETFL, new_option);
  return old_option;
}