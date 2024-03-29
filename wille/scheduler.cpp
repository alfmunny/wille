#include "scheduler.h"
#include "log.h"
#include "macro.h"

namespace wille {

static Logger::ptr g_logger = WILLE_LOG_NAME("system");

static thread_local Scheduler* t_scheduler = nullptr;
static thread_local Fiber* t_fiber = nullptr;

Scheduler::Scheduler(size_t threads, bool use_caller, const std::string& name)
    :m_name(name) {
    Thread::SetName(m_name);
    WILLE_ASSERT(threads > 0);

    if (use_caller) {
        Fiber::GetThis();
        --threads;
        WILLE_ASSERT(GetThis() == nullptr);
        t_scheduler= this;
        m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));
        t_fiber = m_rootFiber.get();
        m_rootThread = GetThreadId();
        m_threadIds.push_back(m_rootThread);
    } else {
        m_rootThread = -1;
    }
    m_threadCount = threads;
};

Scheduler::~Scheduler() {
    WILLE_ASSERT(m_stopping);
    if (GetThis() == this) {
        t_scheduler = nullptr;
    }
}

Scheduler* Scheduler::GetThis() { return t_scheduler; }

Fiber* Scheduler::GetMainFiber() { return t_fiber; }

void Scheduler::start() {
    MutexType::Lock lock(m_mutex);
    if (!m_stopping) {
        return;
    }

    m_stopping = false;
    WILLE_ASSERT(m_threads.empty());

    m_threads.resize(m_threadCount);

    for (size_t i = 0; i < m_threadCount; ++i) {
        m_threads[i].reset(new Thread(std::bind(&Scheduler::run, this),
                                      m_name + "_" + std::to_string(i)));
        m_threadIds.push_back(m_threads[i]->getId());
    }
}
void Scheduler::stop() {
    m_autoStop = true;

    if (m_rootFiber && m_threadCount == 0 &&
        (m_rootFiber->getState() == Fiber::TERM ||
         m_rootFiber->getState() == Fiber::INIT)) {
        WILLE_LOG_INFO(g_logger) << this << " stopped";
        m_stopping = true;

        if (stopping()) {
            return;
        }
    }

    if (m_rootThread != -1) {
        WILLE_ASSERT(GetThis() == this);
    } else {
        WILLE_ASSERT(GetThis() != this);
    }

    m_stopping = true;
    for (size_t i = 0; i < m_threadCount; ++i) {
        tickle();
    }

    if (m_rootFiber) {
        tickle();
    }

    if(m_rootFiber) {
        if(!stopping()) {
            m_rootFiber->swapIn();
        }
    }

    std::vector<Thread::ptr> thrs;
    {
        MutexType::Lock lock(m_mutex);
        thrs.swap(m_threads);
    }

    for(auto& i : thrs) {
        i->join();
    }
}

void Scheduler::setThis() { t_scheduler = this; }

void Scheduler::run() {
    Fiber::GetThis();
    setThis();

    if (GetThreadId() != m_rootThread) {
        t_fiber = Fiber::GetThis().get();
    }

    Fiber::ptr idle_fiber(new Fiber(std::bind(&Scheduler::idle, this)));
    Fiber::ptr cb_fiber;
    Task tk;

    while(true) {
        tk.reset();
        bool tickle_me = false;
        bool is_active = false;
        {
            MutexType::Lock lock(m_mutex);

            auto it = m_tasks.begin();
            while(it != m_tasks.end()) {
                // Check if it should run on a specific thread
                if(it->thread != -1 && it->thread != wille::GetThreadId()) {
                    ++it;
                    continue;
                }

                WILLE_ASSERT(it->fiber || it->cb);
                if(it->fiber && it->fiber->getState() == Fiber::EXEC) {
                    ++it;
                    continue;
                }

                tk = *it;
                m_tasks.erase(it);
                tickle_me = true;
                break;
            }
        }

        if(tickle_me) {
            ++m_activeThreadCount;
            is_active = true;
            tickle();
        }

        if(tk.fiber && !tk.fiber->stateTermOrExcept()) {
            tk.fiber->swapIn();
            --m_activeThreadCount;

            if(tk.fiber->getState() == Fiber::READY) {
                schedule(tk.fiber);
            } else if (!tk.fiber->stateTermOrExcept()) {
                tk.fiber->setState(Fiber::HOLD);
            }
            tk.reset();
        } else if(tk.cb) {
            if(cb_fiber) {
                cb_fiber->reset(tk.cb);
            } else {
                cb_fiber.reset(new Fiber(tk.cb));
            }
            tk.reset();
            cb_fiber->swapIn();
            --m_activeThreadCount;

            if(cb_fiber->getState() == Fiber::READY) {
                schedule(cb_fiber);
                cb_fiber.reset();
            } else if(cb_fiber->stateTermOrExcept()) {
                cb_fiber->reset(nullptr);
            } else { // if(cb_fiber->getState != Fiber::TERM)
                cb_fiber->setState(Fiber::HOLD);
                cb_fiber.reset();
            }
        } else {
            if(is_active) {
                --m_activeThreadCount;
                continue;
            }

            if(idle_fiber->getState() == Fiber::TERM) {
                WILLE_LOG_INFO(g_logger) << "idle fiber terminate";
                break;
            } 
            ++m_idleThreadCount;
            idle_fiber->swapIn();
            --m_idleThreadCount;
            if(!idle_fiber->stateTermOrExcept()) {
                idle_fiber->setState(Fiber::HOLD);
            }
        }
    }
}

void Scheduler::tickle() {
    WILLE_LOG_INFO(g_logger) << "tickle";
}

bool Scheduler::stopping() {
    MutexType::Lock lock(m_mutex);
    return m_autoStop && m_stopping
        && m_tasks.empty() && m_activeThreadCount == 0;
}

void Scheduler::idle() {
    WILLE_LOG_INFO(g_logger) << "idle";
    while(!stopping()) {
        Fiber::YieldToHold();
    }
}

} // namespace wille
