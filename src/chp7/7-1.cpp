/**
 * @file 7-1.cpp
 * @author balance (2570682750@qq.com)
 * @brief 测试进程的UID和EUID的区别
 * @version 0.1
 * @date 2022-08-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <stdio.h>
#include <unistd.h>

int main() {
  uid_t uid = getuid();
  uid_t euid = geteuid();
  printf("userid is : %d, effective userid is:%d\n", uid, euid);
  return 0;
}