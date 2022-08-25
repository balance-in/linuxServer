/**
 * @file 7-2.cpp
 * @author balance (2570682750@qq.com)
 * @brief 以root身份启动的进程切换为以一个普通用户身份运行
 * @version 0.1
 * @date 2022-08-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <unistd.h>

static bool switch_to_user(uid_t user_id, gid_t gp_id) {
  if (user_id == 0 && gp_id == 0) {
    return false;
  }

  //确保当前用户是合法用户：root或者目标用户
  gid_t gid = getgid();
  uid_t uid = getuid();
  if (((gid != 0) || (uid != 0)) && ((gid != gp_id) || (uid != user_id))) {
    return false;
  }

  if (uid != 0) {
    return true;
  }

  if ((setgid(gp_id) < 0) || (setuid(user_id) < 0)) {
    return false;
  }
  return true;
}