#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <string>

#include "ThreadPool.hpp"

using std::cout, std::endl;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void func(std::string url) {
    pthread_mutex_lock(&mutex);
    cout << "Working on " << url << endl;
    pthread_mutex_unlock(&mutex);

    // Some work
    sleep(3);

    pthread_mutex_lock(&mutex);
    cout << "Finished " << url << endl;
    pthread_mutex_unlock(&mutex);
}

int main() {
    ThreadPool pool = ThreadPool(5);

    for (int i = 0; i < 20; ++i) {
        std::string url = "google.com/" + std::to_string(i);
        pool.queue(Task(func, url));
    }

    pool.wait();
}
