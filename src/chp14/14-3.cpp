/**
 * @file 14-3.cpp
 * @author balance (2570682750@qq.com)
 * @brief 在多线程程序中调用fork函数
 * @version 0.1
 * @date 2022-09-21
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
pthread_mutex_t mutex;
void prepare() { pthread_mutex_lock(&mutex); }
void infork() { pthread_mutex_unlock(&mutex); }
/*子线程运行的函数。它首先获得互斥锁mutex，然后暂停5 s，再释放该互斥锁*/
void *another(void *arg) {
  printf("in child thread, lock the mutex");
  pthread_mutex_lock(&mutex);
  sleep(5);
  pthread_mutex_unlock(&mutex);
  return NULL;
}

int main() {
  pthread_mutex_init(&mutex, NULL);
  pthread_t id;
  pthread_create(&id, NULL, another, NULL);
  /*父进程中的主线程暂停1 s，以确保在执行fork操作之前，子线程已经开始运行并获
得了互斥变量mutex*/
  sleep(1);
  // pthread提供了一个专门的函数pthread_atfork，以确保fork
  // 调用后父进程和子进程都拥有一个清楚的锁状态
  int pid = pthread_atfork(prepare, infork, infork);
  if (pid < 0) {
    pthread_mutex_destroy(&mutex);
    return 1;
  } else if (pid == 0) {
    printf("I am in the child,want to get the lock\n");
    /*子进程从父进程继承了互斥锁mutex的状态，该互斥锁处于锁住的状态，这是由父进
    程中的子线程执行pthread_mutex_lock引起的，因此，下面这句加锁操作会一直阻塞，
    尽管从逻辑上来说它是不应该阻塞的*/
    pthread_mutex_lock(&mutex);
    printf("I can not run to here,oop...\n");
    pthread_mutex_unlock(&mutex);
    exit(0);
  } else {
    wait(NULL);
  }
  pthread_join(id, NULL);
  pthread_mutex_destroy(&mutex);
  return 0;
}