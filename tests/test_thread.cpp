#include <wille/thread.h>
#include <wille/mutex.h>
#include <vector>
#include <wille/log.h>
#include <iostream>
#include <thread>
#include <wille/config.h>

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

    YAML::Node root = YAML::LoadFile("/Users/alfmunny/Projects/wille/config/log.yml");
    wille::Config::LoadFromYaml(root);

    for (int i = 0; i < n_thread; ++i) {
        thrs.push_back(wille::Thread::ptr(new wille::Thread(*fun1, "name" + std::to_string(i))));
    }

    for(int i = 0; i < n_thread; ++i) {
        thrs[i]->join();
    }

    WILLE_LOG_INFO(logger) << "count=" << count;
    WILLE_LOG_INFO(logger) << "tread test end";

    return 0;

}
