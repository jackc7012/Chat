#ifndef __MY_THREAD_POOL__
#define __MY_THREAD_POOL__

#include "public.h"
#include <vector>
#include <queue>
#include <thread>

template <typename T>
class CThreadPool {
  public:
    CThreadPool(unsigned int pool_sum);
    ~CThreadPool();

    bool AddTask(const s_HandleRecv &task);

    bool GetTask();

  private:
    std::queue<T*> task_queue;
    std::vector<std::thread> work_thread;
};


#endif //__MY_THREAD_POOL__

