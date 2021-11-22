#include "util.h"
#include "fiber.h"
#include <pthread.h>
#include <execinfo.h>
#include "log.h"
#include <sys/time.h>


namespace wille {

static wille::Logger::ptr g_logger = WILLE_LOG_NAME("system");

#ifdef __APPLE__
uint64_t GetThreadId() {
    uint64_t tid;
    pthread_threadid_np(NULL, &tid);
    return tid;
}
#elif __linux
pid_t GetThreadId() {
    return syscall(SYS_gettid);
}
#endif

uint32_t GetFiberId() {
    return wille::Fiber::GetFiberId();
}

void Backtrace(std::vector<std::string>& bt, int size, int skip = 1) {

    void** array = (void**)malloc((sizeof(void*) * size));
    size_t s = ::backtrace(array, size);

    char** strings = backtrace_symbols(array, s);

    if(strings==NULL) {
        WILLE_LOG_ERROR(g_logger) << "backtrace_symbols error";
        free(strings);
        free(array);
        return;
    }

    for(size_t i = skip; i < s; ++i) {
        bt.push_back(strings[i]);
    }

    free(strings);
    free(array);
}

std::string BacktraceToString(int size, int skip, const std::string& prefix) {
    std::vector<std::string> bt;
    Backtrace(bt, size, skip);
    std::stringstream ss;
    for (size_t i = 0; i < bt.size(); ++i) {
        ss << prefix << bt[i] << std::endl;
    }
    return ss.str();
}

uint64_t GetCurrentMS() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000ul  + tv.tv_usec / 1000;
}

}
