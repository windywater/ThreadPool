#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include <list>
#include <queue>
#include <semaphore.h>

class ThreadPool
{
public:
    ThreadPool(std::size_t threadCount, std::size_t maxQueueCount);
    ~ThreadPool();
    
    typedef void*(*ThreadFunc)(void*);
    struct ThreadTask
    {
        ThreadFunc function;
        void* argument;
    };
    
    bool addThread(ThreadFunc thread, void* arg);
    
private:
    void destroy();
    static void* threadPoolWorker(void* arg);
    
private:
    std::size_t m_threadCount;
    std::size_t m_maxQueueCount;
    std::queue<ThreadTask> m_taskQueue;
    sem_t m_sem;
    pthread_mutex_t m_resLock;
    std::list<pthread_t> m_threads;
    bool m_shutdown;
};

#endif // THREADPOOL_H
