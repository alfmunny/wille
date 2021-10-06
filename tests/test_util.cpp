#include <wille/wille.h>
#include <assert.h>

wille::Logger::ptr g_logger = WILLE_LOG_ROOT();

void fun1() {
    //WILLE_LOG_INFO(g_logger) << wille::BacktraceToString(10);
    WILLE_ASSERT(false);
}

int main(int argc, char** argv) {
    fun1();
    return 0;
}

