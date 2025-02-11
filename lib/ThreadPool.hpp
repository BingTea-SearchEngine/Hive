#include <functional>
#include <iostream>
#include <optional>
#include <pthread.h>
#include <queue>


typedef struct Task {
    std::function<void()> func;

    Task() = delete;

    template<typename F, typename... Args>
    Task(F&& f, Args&&... args) {
        func = [f = std::forward<F>(f), tup = std::make_tuple(std::forward<Args>(args)...)]() mutable {
            std::apply(f, tup);
        };
    }

} Task;

struct Thread {
    pthread_t t;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
    bool end = false;
};

struct FuncWrapperArgs {
    Thread* me;
    std::queue<Task>* taskQueue;
    pthread_mutex_t* queueMutex;
    pthread_cond_t* queueCV;
    size_t* currentTasks;
    pthread_mutex_t* currentTasksMutex;
    pthread_cond_t* currentTasksCV;
};

class ThreadPool {
public:

    // Disable default constructor
    ThreadPool() = delete;

    // Initializes a thread pool of numThreads and starts the matching thread
    ThreadPool(size_t numThreads);

    ~ThreadPool();

    // Blocks while queue is two times the size of the number of threads
    void queue(Task t);

    // Blocks until all tasks in queue and current tasks finish
    void wait();

private:
    std::queue<Task> _queue;
    size_t _currentTasks = 0;
    std::vector<FuncWrapperArgs*> _threads;

    pthread_mutex_t _queueMutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t _queueCV = PTHREAD_COND_INITIALIZER;

    pthread_mutex_t _currentTasksMutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t _currentTasksCV = PTHREAD_COND_INITIALIZER;
};

// Function that wraps around user defined task. Waits while Task* task is a nullptr. Never returns
void* funcWrapper(void *args);

