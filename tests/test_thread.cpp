#include <wille/thread.h>
#include <vector>
#include <wille/log.h>
#include <iostream>
#include <thread>

wille::Logger::ptr logger = WILLE_LOG_NAME("system");

void fun1() {
    WILLE_LOG_INFO(logger) << "name: " << wille::Thread::GetName()
        << " this.name: " << wille::Thread::GetThis()->getName()
        << " id: " << wille::GetThreadId()
        << " this.id: " << wille::Thread::GetThis()->getId();
    sleep(30);
}

void fun2() {
    WILLE_LOG_INFO(logger) << "name: this is a log line very long and long";
}

int main() {
    WILLE_LOG_INFO(logger) << "tread test begin";
    std::vector<wille::Thread::ptr> thrs;
    std::vector<std::thread> thr_v(5);

    for (int i = 0; i < 5; ++i) {
        //thrs.push_back(wille::Thread::ptr(new wille::Thread(*fun1, "name" + std::to_string(i))));
        //thr_v.push_back(std::thread(fun2));
        thr_v[i] = std::thread(fun2);
    }

    for(int i = 0; i < 5; ++i) {
        //thrs[i]->join();
        thr_v[i].join();
    }

    WILLE_LOG_INFO(logger) << "tread test end";
    return 0;
}
