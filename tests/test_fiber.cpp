#include "wille/wille.h"

wille::Logger::ptr g_logger = WILLE_LOG_ROOT();

void run_in_fiber() {
    WILLE_LOG_INFO(g_logger) << "run_in_fiber begin";
    wille::Fiber::YieldToHold();
    WILLE_LOG_INFO(g_logger) << "run_in_fiber end";
    wille::Fiber::YieldToHold();
}

void test_fiber() {
    WILLE_LOG_INFO(g_logger) << "main begin -1";

    {
        wille::Fiber::GetThis();
        WILLE_LOG_INFO(g_logger) << "main begin";
        wille::Fiber::ptr fiber(new wille::Fiber(run_in_fiber));
        fiber->swapIn();
        WILLE_LOG_INFO(g_logger) << "main after swapIn";
        fiber->swapIn();
        WILLE_LOG_INFO(g_logger) << "main after end";
        fiber->swapIn();
    }

    WILLE_LOG_INFO(g_logger) << "main after end 2";
}

int main(int argc, char** argv) {
    wille::Thread::SetName("main");

    std::vector<wille::Thread::ptr> thrs;
    for(int i = 0; i < 3; ++i) {
        thrs.push_back(wille::Thread::ptr(
                    new wille::Thread(&test_fiber, "name_" + std::to_string(i))
                    ));
    }

    for(auto i : thrs) {
        i->join();
    }

    return 0;
}


