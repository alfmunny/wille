#include <wille/thread.h>
#include <wille/mutex.h>
#include <vector>
#include <wille/log.h>
#include <iostream>
#include <thread>

wille::Logger::ptr logger = WILLE_LOG_NAME("system");
int count = 0;
wille::RWMutex rw_mutex;
wille::Mutex mutex;

void fun1() {
    WILLE_LOG_INFO(logger) << "name: " << wille::Thread::GetName()
        << " this.name: " << wille::Thread::GetThis()->getName()
        << " id: " << wille::GetThreadId()
        << " this.id: " << wille::Thread::GetThis()->getId();
}

void fun2() {
    //wille::Mutex::Lock lock(mutex);
    //wille::RWMutex::ReadLock lock(rw_mutex);
    wille::RWMutex::WriteLock lock(rw_mutex);
    for (int i = 0; i < 10000000; ++i) {
        ++count;
    }
}

int main() {
    WILLE_LOG_INFO(logger) << "tread test begin";
    std::vector<wille::Thread::ptr> thrs;
    std::vector<std::thread> thr_v(5);
    int n_thread = 5;

    for (int i = 0; i < n_thread; ++i) {
        thrs.push_back(wille::Thread::ptr(new wille::Thread(*fun2, "name" + std::to_string(i))));
        //thr_v.push_back(std::thread(fun2));
        //thr_v[i] = std::thread(fun2);
    }

    for(int i = 0; i < n_thread; ++i) {
        thrs[i]->join();
        //thr_v[i].join();
    }

    WILLE_LOG_INFO(logger) << count;
    WILLE_LOG_INFO(logger) << "tread test end";
    return 0;
}
