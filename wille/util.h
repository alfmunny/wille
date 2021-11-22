#ifndef __WILLE_UTIL_H__
#define __WILLE_UTIL_H__

#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <string>
#include <vector>

namespace wille {


#ifdef __APPLE__
uint64_t GetThreadId();
#elif __linux
pid_t GetThreadId();
#endif

uint32_t GetFiberId();

void Backtrace(std::vector<std::string>& bt, int size, int skip);

std::string BacktraceToString(int size = 64, int skip = 2, const std::string& prefix = "");

uint64_t GetCurrentMS();

}


#endif
