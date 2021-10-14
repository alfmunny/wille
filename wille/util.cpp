#include "util.h"
#include "fiber.h"
#include <pthread.h>
#include <execinfo.h>
#include "log.h"


namespace wille {

static wille::Logger::ptr g_logger = WILLE_LOG_NAME("system");

uint64_t GetThreadId() {
    uint64_t tid;
    pthread_threadid_np(NULL, &tid);
    return tid;
}

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
}
