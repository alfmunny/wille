#ifndef __WILLE_MAROCH_H__
#define __WILLE_MAROCH_H__
#include "log.h"
#include "util.h"
#include <assert.h>

#define WILLE_ASSERT(x) \
    if(!(x)) { \
        WILLE_LOG_ERROR(WILLE_LOG_ROOT()) << "ASSERTION: " #x \
                << "\nbacktrace:\n" \
                << wille::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }
 

#define WILLE_ASSERT2(x, w) \
    if(!(x)) { \
        WILLE_LOG_ERROR(WILLE_LOG_ROOT()) << "ASSERTION: " #x \
                << "\n" << w \
                << "\nbacktrace:\n" \
                << wille::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }

#endif
