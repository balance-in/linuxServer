/**
 * @file 6-5.cpp
 * @author balance (2570682750@qq.com)
 * @brief 同时输出数据到终端和文件程序
 * @version 0.1
 * @date 2022-08-25
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
#include <sys/uio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s<file>\n", argv[0]);
    return 1;
  }
  int filefd = open(argv[1], O_CREAT | O_WRONLY | O_TRUNC, 0666);
  assert(filefd > 0);
  int pipefd_stdout[2];
  int ret = pipe(pipefd_stdout);
  assert(ret != -1);
  int pipefd_file[2];
  ret = pipe(pipefd_file);
  assert(ret != -1);

  ret = splice(STDIN_FILENO, NULL, pipefd_stdout[1], NULL, 32768,
               SPLICE_F_MORE | SPLICE_F_MOVE);
  assert(ret != -1);
  ret = tee(pipefd_stdout[0], pipefd_file[1], 32768, SPLICE_F_NONBLOCK);
  assert(ret != -1);
  ret = splice(pipefd_file[0], NULL, filefd, NULL, 32768,
               SPLICE_F_MORE | SPLICE_F_MOVE);
  assert(ret != -1);
  ret = splice(pipefd_stdout[0], NULL, STDIN_FILENO, NULL, 32768,
               SPLICE_F_MORE | SPLICE_F_MOVE);
  assert(ret != -1);
  close(filefd);
  close(pipefd_file[0]);
  close(pipefd_file[1]);
  close(pipefd_stdout[0]);
  close(pipefd_stdout[1]);
  return 0;
}