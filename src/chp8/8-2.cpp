/**
 * @file 8-2.cpp
 * @author balance (2570682750@qq.com)
 * @brief 带状态转移的状态机
 * @version 0.1
 * @date 2022-09-01
 *
 * @copyright Copyright (c) 2022
 *
 */

/**
STATE_MACHINE() {
  State cur_State = type_A;
  while (cur_State != type_C) {
    Package_pack = getNewPackage();
    switch (cur_State) {
      case type_A:
        process_package_state_A(_pack);
        cur_State = type_B;
        break;
      case type_B:
        process_package_state_B(_pack);
        cur_State = type_C;
        break;
    }
  }
}
**/