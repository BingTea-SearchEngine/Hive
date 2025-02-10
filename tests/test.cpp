#include <gtest/gtest.h>
#include "ThreadPool.hpp"

TEST(Tests, PoolConstructor) {
    ThreadPool pool = ThreadPool(5);
}
