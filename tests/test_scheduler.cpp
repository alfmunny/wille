#include "wille/wille.h"
#include "wille/scheduler.h"

wille::Logger::ptr g_logger = WILLE_LOG_ROOT();

void func() {
    WILLE_LOG_INFO(g_logger) << "test fiber";
}

int main(int argc, char** argv) {
    WILLE_LOG_INFO(g_logger) << "main";
    wille::Scheduler sc(3, false, "test");
    sc.start();
    sleep(2);
    WILLE_LOG_INFO(g_logger) << "schedule";
    sc.schedule(&func);
    sleep(2);
    sc.stop();
    WILLE_LOG_INFO(g_logger) << "over";
    return 0;
}
