#include "util.h"
#include <pthread.h>

namespace wille {

uint64_t GetThreadId() {
    uint64_t tid;
    pthread_threadid_np(NULL, &tid);
    return tid;
}

uint32_t GetFiberId() {
    return 0;
}
}
