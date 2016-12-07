#include "ThreadPool.h"

ThreadPool::ThreadPool(std::size_t threadCount, std::size_t maxQueueCount)
    : m_threadCount(threadCount),
      m_maxQueueCount(maxQueueCount),
      m_shutdown(false)
{
    sem_init(&m_sem, 0, 0);
    pthread_mutex_init(&m_resLock, NULL);
    
    for (std::size_t i = 0; i < m_threadCount; i++)
    {
        pthread_t thread;
        pthread_create(&thread, NULL, ThreadPool::threadPoolWorker, this);
        m_threads.push_back(thread);
    }
}

ThreadPool::~ThreadPool()
{
    destroy();
}

bool ThreadPool::addThread(ThreadFunc thread, void* arg)
{
    pthread_mutex_lock(&m_resLock);
    if (m_taskQueue.size() >= m_maxQueueCount)
    {
        pthread_mutex_unlock(&m_resLock);
        return false;
    }
    
    ThreadTask task = { thread, arg };
    m_taskQueue.push(task);
    
    pthread_mutex_unlock(&m_resLock);
    sem_post(&m_sem);
    
    return true;
}

void ThreadPool::destroy()
{
    m_shutdown = true;
    sem_post(&m_sem);
    sem_destroy(&m_sem);
    
    void* ret;
    std::list<pthread_t>::const_iterator it = m_threads.begin();
    for (; it != m_threads.end(); it++)
        pthread_join(*it, &ret);
}

/*static*/ void* ThreadPool::threadPoolWorker(void* arg)
{
    ThreadPool* pool = (ThreadPool*)arg;
    
    while (true)
    {
        sem_wait(&pool->m_sem);
        if (pool->m_shutdown)
            break;
        
        pthread_mutex_lock(&pool->m_resLock);
        ThreadTask task = pool->m_taskQueue.front();
        pool->m_taskQueue.pop();
        pthread_mutex_unlock(&pool->m_resLock);
        
        (*(task.function))(task.argument);
    }
    
    sem_post(&pool->m_sem);
    
    return 0;    
}
