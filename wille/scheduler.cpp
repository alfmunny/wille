#include "scheduler.h"
#include "log.h"
#include "macro.h"

namespace wille {
static wille::Logger::ptr g_logger = WILLE_LOG_NAME("system");

static thread_local Scheduler* t_scheuler = nullptr;
static thread_local Fiber* t_fiber = nullptr;

Scheduler::Scheduler(size_t threads, bool use_caller, const std::string& name)
    :m_name(name) {
    WILLE_ASSERT(threads > 0);

    if (use_caller) {
        wille::Fiber::GetThis();
        --threads;
        WILLE_ASSERT(GetThis() == nullptr);
        t_scheuler = this;
        m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this)));
        Thread::SetName(name);
        t_fiber = m_rootFiber.get();
        m_rootThread = wille::GetThreadId();
        m_threadIds.push_back(m_rootThread);
    } else {
        m_rootThread = -1;
    }
    m_threadCount = threads;
};

Scheduler::~Scheduler() {
    WILLE_ASSERT(m_stopping);
    if (GetThis() == this) {
        t_scheuler = nullptr;
    }
}

Scheduler* Scheduler::GetThis() { return t_scheuler; }

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
    m_stopping = true;

    if (m_rootFiber && m_threadCount == 0 &&
        (m_rootFiber->getState() == Fiber::TERM ||
         m_rootFiber->getState() == Fiber::INIT)) {
        WILLE_LOG_INFO(g_logger) << this << " stopped";

        if (stopping()) {
            return;
        }
    }

    if (m_rootThread != -1) {
        WILLE_ASSERT(GetThis() == this);
    } else {
        WILLE_ASSERT(GetThis() != this);
    }

    for (size_t i = 0; i < m_threadCount; ++i) {
        tickle();
    }

    if (m_rootFiber) {
        tickle();
    }

    if(m_rootFiber) {
        if(!stopping()) {
            m_rootFiber->call();
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
    // if(exit_on_this_fiber) {

    //}
}

void Scheduler::setThis() { t_scheuler = this; }

void Scheduler::run() {
    Fiber::GetThis();
    setThis();
    if (wille::GetThreadId() != m_rootThread) {
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
                if(it->thread != -1 && it->thread != wille::GetThreadId()) {
                    ++it;
                    tickle_me = true;
                    continue;
                }

                WILLE_ASSERT(it->fiber || it->cb);
                if(it->fiber && it->fiber->getState() == Fiber::EXEC) {
                    ++it;
                    continue;
                }
                tk = *it;
                m_tasks.erase(it);
                ++m_activeThreadCount;
                is_active = true;
                break;
            }
        }

        if(tickle_me) {
            tickle();
        }

        if(tk.fiber && tk.fiber->getState() != Fiber::TERM 
                && tk.fiber->getState() != Fiber::EXCEPT) {

            tk.fiber->swapIn();
            --m_activeThreadCount;

            if(tk.fiber->getState() == Fiber::READY) {
                schedule(tk.fiber);
            } else if (tk.fiber->getState() != Fiber::TERM
                    && tk.fiber->getState() != Fiber::EXCEPT) {
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
            } else if(cb_fiber->getState() == Fiber::EXCEPT
                    || cb_fiber->getState() == Fiber::TERM) {
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
            if(idle_fiber->getState() != Fiber::TERM
                    && idle_fiber->getState() != Fiber::EXCEPT) {
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
        wille::Fiber::YieldToHold();
    }
}

} // namespace wille
