/**
 * @file locker.h
 * @author balance (2570682750@qq.com)
 * @brief 3种线程同步机制分别封装成3个类
 * @version 0.1
 * @date 2022-09-21
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef LOCKER_H
#define LOCKER_H
#include <pthread.h>
#include <semaphore.h>

#include <exception>
/*封装信号量的类*/
class sem {
 private:
  sem_t m_sem;

 public:
  //创建并初始化信号量
  sem() {
    if (sem_init(&m_sem, 0, 0) != 0) {
      /*构造函数没有返回值，可以通过抛出异常来报告错误*/
      throw std::exception();
    }
  }
  /*销毁信号量*/
  sem::~sem() { sem_destroy(&m_sem); }
  /*等待信号量*/
  bool wait() { return sem_wait(&m_sem) == 0; }
  /*增加信号量*/
  bool post() { return sem_post(&m_sem) == 0; }
};
/*封装互斥锁的类*/
class locker {
 private:
  pthread_mutex_t m_mutex;

 public:
  /*创建并初始化互斥锁*/
  locker() {
    if (pthread_mutex_init(&m_mutex, NULL) != 0) {
      throw std::exception();
    }
  }
  /*销毁互斥锁*/
  ~locker() { pthread_mutex_destroy(&m_mutex); }
  /*获取互斥锁*/
  bool lock() { return pthread_mutex_lock(&m_mutex); }
  /*释放互斥锁*/
  bool unlock() { return pthread_mutex_unlock(&m_mutex) == 0; }
};
/*封装条件变量的类*/
class cond {
 private:
  pthread_mutex_t m_mutex;
  pthread_cond_t m_cond;

 public:
  /*创建并初始化条件变量*/
  cond() {
    if (pthread_mutex_init(&m_mutex, NULL) != 0) {
      throw std::exception();
    }
    if (pthread_cond_init(&m_cond, NULL) != 0) {
      /*构造函数中一旦出现问题，就应该立即释放已经成功分配了的资源*/
      pthread_mutex_destroy(&m_mutex);
      throw std::exception();
    }
  }
  /*销毁条件变量*/
  ~cond() {
    pthread_mutex_destroy(&m_mutex);
    pthread_cond_destroy(&m_cond);
  }
  /*等待条件变量*/
  bool wait() {
    int ret = 0;
    pthread_mutex_lock(&m_mutex);
    ret = pthread_cond_wait(&m_cond, &m_mutex);
    pthread_mutex_unlock(&m_mutex);
    return ret == 0;
  }
  /*唤醒等待条件变量的线程*/
  bool signal() { return pthread_cond_signal(&m_cond) == 0; }
};

#endif
