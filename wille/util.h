#ifndef __WILLE_UTIL_H__
#define __WILLE_UTIL_H__

#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <stdio.h>

namespace wille {

uint64_t GetThreadId();

uint32_t GetFiberId();

}

#endif
