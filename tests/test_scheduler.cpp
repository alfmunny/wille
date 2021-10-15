#include "wille/wille.h"
#include "wille/scheduler.h"

wille::Logger::ptr g_logger = WILLE_LOG_ROOT();

void func() {
    WILLE_LOG_INFO(g_logger) << "test in fiber";
    static int count = 5;
    sleep(1);
    if (--count >= 0) {
        wille::Scheduler::GetThis()->schedule(&func, wille::GetThreadId());
    }
}

int main(int argc, char** argv) {
    WILLE_LOG_INFO(g_logger) << "main";
    wille::Scheduler sc(3, false, "test");
    sc.start();
    WILLE_LOG_INFO(g_logger) << "schedule";
    sc.schedule(&func);
    WILLE_LOG_INFO(g_logger) << "schedule finish";
    sc.stop();
    WILLE_LOG_INFO(g_logger) << "over";
    return 0;
}
