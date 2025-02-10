#include <iostream>
#include <unistd.h>

#include "ThreadPool.hpp"

using std::cout, std::endl;

void func() {
    cout << "In func" << endl;
}

int main() {
    ThreadPool pool = ThreadPool(10);

    pool.queue(Task(func));
    pool.queue(Task(func));
    pool.queue(Task(func));
    pool.queue(Task(func));
    pool.queue(Task(func));
    pool.queue(Task(func));
    pool.queue(Task(func));
    pool.queue(Task(func));
    pool.queue(Task(func));
    pool.queue(Task(func));
    pool.queue(Task(func));
    pool.queue(Task(func));

    pool.wait();
}
