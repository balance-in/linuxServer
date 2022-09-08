/**
 * @file 8-3.cpp
 * @author balance (2570682750@qq.com)
 * @brief HTTP请求的读取和分析
 * @version 0.1
 * @date 2022-09-01
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#define BUFFER_SIZE 4096

//主状态机的两种可能状态，分别表示：当前正在分析请求行，当前正在分析头部字段
enum CHECK_STATE{ CHECK_STATE_REQUESTLINE = 0, CHECK_STATE_HEADER};
//从状态机的三种可能状态，即行的读取状态，分别表示：读取到一个完整的行、行出错和行数据尚且不完整
enum LINE_STATUS{LINE_OK = 0, LINE_BAD, LINE_OPEN};

