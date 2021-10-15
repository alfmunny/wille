#include "fiber.h"
#include "macro.h"
#include "config.h"
#include "scheduler.h"
#include <atomic>

namespace wille {

static std::atomic<uint64_t> s_fiber_id {0};
static std::atomic<uint64_t> s_fiber_count {0};
static thread_local Fiber* t_fiber = nullptr;
static thread_local Fiber::ptr t_threadFiber = nullptr;

static ConfigVar<uint32_t>::ptr g_fiber_stack_size = Config::Lookup<uint32_t>("fiber.stack_size", 128 * 1024, "fiber stack size");

static Logger::ptr g_logger = WILLE_LOG_NAME("system");

class MallocStackAllocator {
public:
    static void* Alloc(size_t size) {
        return malloc(size);
    }

    static void Dealloc(void *vp, size_t size) {
        free(vp);
    }
};
using StackAllocator = MallocStackAllocator;

uint64_t Fiber::GetFiberId() {
    if(t_fiber) {
        return t_fiber->getId();
    }
    return 0;
}

Fiber::Fiber() {
    m_state = EXEC;
    SetThis(this);

    if(getcontext(&m_ctx)) {
        WILLE_ASSERT2(false, "getcontext");
    }
    ++s_fiber_count;

    WILLE_LOG_DEBUG(g_logger) << "Fiber::Fiber main" ;
}


Fiber::Fiber(std::function<void()> cb, size_t stacksize, bool use_caller) 
    :m_id(++s_fiber_id)
    ,m_cb(cb) {
    ++s_fiber_count;

    if(getcontext(&m_ctx)) {
        WILLE_ASSERT2(false, "getcontext");
    }

    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = sizeof(m_stack);
    if(!use_caller) {
        makecontext(&m_ctx, &Fiber::MainFunc, 0);
    } else {
        makecontext(&m_ctx, &Fiber::CallerMainFunc, 0);
    }

    WILLE_LOG_DEBUG(g_logger) << "Fiber::Fiber id=" << m_id;
}

Fiber::~Fiber() {
    --s_fiber_count;

    if(this != t_threadFiber.get()) {
        WILLE_ASSERT(m_state == TERM
                || m_state == EXCEPT
                || m_state == INIT)

    } else {
        WILLE_ASSERT(!m_cb);
        WILLE_ASSERT(m_state == EXEC);
        Fiber* cur = t_fiber;
        if(cur == this) {
            SetThis(nullptr);
        }
    }

    WILLE_LOG_DEBUG(g_logger) << "Fiber::~Fiber id=" << m_id;
}

void Fiber::reset(std::function<void()> cb, bool use_caller) {
    WILLE_ASSERT(m_stack);
    WILLE_ASSERT(m_state == TERM 
            || m_state == INIT
            || m_state == EXCEPT);
    m_cb = cb;
    if(getcontext(&m_ctx)) {
        WILLE_ASSERT2(false, "getcontext");
    }
    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = sizeof(m_stack);

    makecontext(&m_ctx, &Fiber::MainFunc, 0);
    m_state = INIT;
};

void Fiber::call() {
    SetThis(this);
    m_state = EXEC;
    if(swapcontext(&t_threadFiber->m_ctx, &m_ctx)) {
        WILLE_ASSERT2(false, "swapcontext");
    }
}

void Fiber::back() {
    SetThis(t_threadFiber.get());
    if(swapcontext(&m_ctx, &t_threadFiber->m_ctx)) {
        WILLE_ASSERT2(false, "swapcontext");
    }
}

void Fiber::swapIn() {
    SetThis(this);
    WILLE_ASSERT(m_state != EXEC);
    m_state = EXEC;
    if(swapcontext(&Scheduler::GetMainFiber()->m_ctx, &m_ctx)) {
        WILLE_ASSERT2(false, "swapcontext");
    }
};

void Fiber::swapOut() {
    SetThis(Scheduler::GetMainFiber());
    if(swapcontext(&m_ctx, &Scheduler::GetMainFiber()->m_ctx)) {
        WILLE_ASSERT2(false, "swapcontext");
    }
};

void Fiber::SetThis(Fiber* f) {
    t_fiber = f;
}

Fiber::ptr Fiber::GetThis() {
    if(t_fiber) {
        return t_fiber->shared_from_this();
    }
    Fiber::ptr main_fiber(new Fiber);
    WILLE_ASSERT(t_fiber == main_fiber.get());
    t_threadFiber = main_fiber;
    return t_fiber->shared_from_this();
}

void Fiber::YieldToReady() {
    Fiber::ptr cur = GetThis();
    WILLE_ASSERT(cur->m_state == EXEC);
    cur->m_state = READY;
    cur->swapOut();
}

void Fiber::YieldToHold() {
    Fiber::ptr cur = GetThis();
    WILLE_ASSERT(cur->m_state == EXEC);
    cur->m_state = HOLD;
    cur->swapOut();
}

uint64_t Fiber::TotalFibers() {
    return s_fiber_count;
}

void Fiber::MainFunc() {
    Fiber::ptr cur = GetThis();
    WILLE_ASSERT(cur);
    try {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    } catch (std::exception& ex) {
        cur->m_state = EXCEPT;
                cur->m_state = EXCEPT;
        WILLE_LOG_ERROR(g_logger) << "Fiber Except: " << ex.what()
            << " fiber_id=" << cur->getId()
            << std::endl
            << wille::BacktraceToString();
        WILLE_LOG_ERROR(g_logger) << "Fiber Except: " << ex.what();
    } catch (...) {
        cur->m_state = EXCEPT;
        WILLE_LOG_ERROR(g_logger) << "Fiber Except";
    }

    auto raw_ptr = cur.get();

    //WILLE_LOG_DEBUG(g_logger) << "Sub fiber reset begin";
    cur.reset();
    //WILLE_LOG_DEBUG(g_logger) << "Sub fiber reset end";
    raw_ptr->swapOut();
    //cur->swapOut();
}

void Fiber::CallerMainFunc() {
    Fiber::ptr cur = GetThis();
    WILLE_ASSERT(cur);
    try {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    } catch (std::exception& ex) {
        cur->m_state = EXCEPT;
                cur->m_state = EXCEPT;
        WILLE_LOG_ERROR(g_logger) << "Fiber Except: " << ex.what()
            << " fiber_id=" << cur->getId()
            << std::endl
            << wille::BacktraceToString();
        WILLE_LOG_ERROR(g_logger) << "Fiber Except: " << ex.what();
    } catch (...) {
        cur->m_state = EXCEPT;
        WILLE_LOG_ERROR(g_logger) << "Fiber Except";
    }

    auto raw_ptr = cur.get();

    //WILLE_LOG_DEBUG(g_logger) << "Sub fiber reset begin";
    cur.reset();
    //WILLE_LOG_DEBUG(g_logger) << "Sub fiber reset end";
    raw_ptr->back();
    //cur->swapOut();
}

} // namespace wille
