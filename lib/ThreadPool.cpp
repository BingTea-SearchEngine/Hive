#include "ThreadPool.hpp"
#include <pthread.h>
#include <iostream>

using std::cout, std::endl;

ThreadPool::ThreadPool(size_t numThreads) {
    for (size_t i = 0; i < numThreads; ++i) {
        FuncWrapperArgs* args = new FuncWrapperArgs{.me = new Thread(),
                                                    .taskQueue = &_queue,
                                                    .queueMutex = &_queueMutex,
                                                    .queueCV = &_queueCV,
                                                    .currentTasks = &_currentTasks,
                                                    .currentTasksMutex = &_currentTasksMutex,
                                                    .currentTasksCV = &_currentTasksCV};
        if (pthread_create(&(args->me->t), nullptr, funcWrapper, (void*)args) !=
            0) {
            throw std::runtime_error("Error creating pthread");
        }
        _threads.push_back(args);
    }
}

ThreadPool::~ThreadPool() {
    for (size_t i = 0; i < _threads.size(); ++i) {
        auto* args = _threads[i];

        pthread_mutex_lock(args->queueMutex);
        args->me->end = true;
        pthread_cond_broadcast(args->queueCV);
        pthread_mutex_unlock(args->queueMutex);

        pthread_join(args->me->t, nullptr);

        pthread_mutex_destroy(&args->me->mutex);
        pthread_cond_destroy(&args->me->cv);
        delete args->me;
        delete args;
    }

    pthread_mutex_destroy(&_queueMutex);
    pthread_cond_destroy(&_queueCV);
};

void ThreadPool::queue(Task t) {
    pthread_mutex_lock(&_queueMutex);
    while (_queue.size() > _threads.size() * 2) {
        pthread_cond_wait(&_queueCV, &_queueMutex);
    }
    _queue.push(t);
    pthread_cond_signal(&_queueCV);
    pthread_mutex_unlock(&_queueMutex);
}

void ThreadPool::wait() {
    pthread_mutex_lock(&_queueMutex);
    while (!_queue.empty()) {
        pthread_cond_wait(&_queueCV, &_queueMutex);
    }
    pthread_mutex_unlock(&_queueMutex);

    pthread_mutex_lock(&_currentTasksMutex);
    while (_currentTasks != 0) {
        pthread_cond_wait(&_currentTasksCV, &_currentTasksMutex);
    }
    pthread_mutex_unlock(&_currentTasksMutex);
}

void* funcWrapper(void* arguments) {
    FuncWrapperArgs* args = static_cast<FuncWrapperArgs*>(arguments);
    Thread* me = args->me;
    while (true) {
        pthread_mutex_lock(args->queueMutex);

        while (args->taskQueue->empty() && !me->end) {
            pthread_cond_wait(args->queueCV, args->queueMutex);
        }

        if (me->end) {
            pthread_mutex_unlock(args->queueMutex);
            pthread_exit(nullptr);
        }

        Task task = std::move(args->taskQueue->front());
        args->taskQueue->pop();
        pthread_cond_broadcast(args->queueCV);
        pthread_mutex_unlock(args->queueMutex);

        pthread_mutex_lock(args->currentTasksMutex);
        ++*(args->currentTasks);
        pthread_mutex_unlock(args->currentTasksMutex);

        task.func();

        pthread_mutex_lock(args->currentTasksMutex);
        --*(args->currentTasks);
        pthread_cond_broadcast(args->currentTasksCV);
        pthread_mutex_unlock(args->currentTasksMutex);
    }
}

