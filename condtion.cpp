//conditionVariablesWithoutPredicate.cpp
#include <condition_variable>
#include <mutex>
#include <thread>
#include <chrono>
#include <iostream>

std::mutex lock;
std::condition_variable condVar;

void waitingForWork() {
    std::this_thread::sleep_for(std::chrono::seconds(0));
    std::cout << "Waiting ..." << std::endl;
    std::unique_lock<std::mutex> l(lock);
    condVar.wait(l);                                     //(1)
    std::cout << "Running ..." << std::endl;
}

void setDataReady() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Data prepared, notify one" << std::endl;
    condVar.notify_one();                                //(2)
}

int main() {
    std::cout << "==========Begin==========" << std::endl;

    std::thread t1(waitingForWork);
    std::thread t2(setDataReady);

    t1.join();
    t2.join();

    std::cout << "===========End===========" << std::endl;
}
