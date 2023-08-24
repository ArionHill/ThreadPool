#include <iostream>

#include "ThreadPool.h"

int add(int i, int j) {
    std::cout << "add" << std::endl;
    return i + j;
}
int multiply(int i, int j) {
    return i * j;
}

int main() {
    ThreadPool thread_pool(3);
    // auto add = [] (int i, int j) {return i + j;};
    auto multiply = [] (int i, int j) {return i * j;};
    

    thread_pool.init();
    std::cout << "b1" << std::endl;
    auto f0 = thread_pool.submit(add, 1, 2);
    std::cout << "b2" << std::endl;
    
    auto f1 = thread_pool.submit(multiply, 2, 3);
    auto res = f0.get();
    std::cout << "ressssssss: " << res << std::endl;
    thread_pool.shutdown();
    std::cout << "multiply: " << f1.get() << std::endl;

}