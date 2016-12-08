# thread pool
```C++
#include "ThreadPool.h"

void* thread(void* arg)
{
    // do something...
    return 0;
}

int main(int argc, char* argv[])
{
    // 4 threads, 10 queue size
    ThreadPool* pool = new ThreadPool(4, 10);

    for (int i = 0; i < 50; i++)
    {
        pool->addThread(thread, i);
    }
    
    sleep(60);
    delete pool;

    return 0;
}
```