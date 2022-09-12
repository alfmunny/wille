#include "wille/wille.h"
#include "wille/scheduler.h"

static wille::Logger::ptr g_logger = WILLE_LOG_ROOT();

void func() {
    static int count = 5;
    WILLE_LOG_INFO(g_logger) << "test in fiber cout=" << count;
    sleep(1);
    if (--count >= 0) {
        //wille::Scheduler::GetThis()->schedule(&func, wille::GetThreadId()); //run in same thread
        wille::Scheduler::GetThis()->schedule(&func); // run in random thread
    }
}

int main(int argc, char** argv) {
    WILLE_LOG_INFO(g_logger) << "main";
    wille::Scheduler sc(1, true, "test");
    sc.start();
    sleep(2);
    WILLE_LOG_INFO(g_logger) << "schedule func";
    sc.schedule(&func);
    sleep(2);
    sc.stop();
    WILLE_LOG_INFO(g_logger) << "main over";
    return 0;
}
