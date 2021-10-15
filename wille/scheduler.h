#ifndef __WILLE_SCHEDULER_H___
#define __WILLE_SCHEDULER_H___
#include <memory>
#include "mutex.h"
#include "fiber.h"
#include "thread.h"
#include <list>
#include <vector>
#include "log.h"

namespace wille {
class Scheduler {
public:
    typedef std::shared_ptr<Scheduler> ptr;
    typedef Mutex MutexType;

    Scheduler(size_t threads = 1, bool use_caller = true, const std::string& name = "");
    virtual ~Scheduler();

    const std::string& getName() const { return m_name; }

    static Scheduler* GetThis();
    static Fiber* GetMainFiber();
    static std::string GetName();

    void start();
    void stop();

    template<class FiberOrCb>
    void schedule(FiberOrCb fc, int thread = -1) {
        bool need_tickle = false;
        {
            MutexType::Lock lock(m_mutex);
            need_tickle = scheduleNoLock(fc, thread);
        }

        if(need_tickle) {
            tickle();
        }
    }

    template<class InputIterator>
        void schedule(InputIterator begin, InputIterator end) {
            bool need_tickle = false;
            {
                MutexType::Lock lock(m_mutex);
                while(begin != end) {
                    need_tickle = scheduleNoLock(&*begin) || need_tickle;
                    ++begin;
                }
            }

            if(need_tickle) {
                tickle();
            }

        }

protected:
    virtual void tickle();
    virtual bool stopping();
    virtual void idle();
    void run();
    void setThis();
private:
    template<class FiberOrCb>
    bool scheduleNoLock(FiberOrCb fc, int thread) {
        bool need_tickle = m_tasks.empty();
        Task ft(fc, thread); 
        if(ft.fiber || ft.cb) {
            m_tasks.push_back(ft);
        }
        return need_tickle;
    }

private:
    struct Task {
        Fiber::ptr fiber;
        std::function<void()> cb;
        int thread;

        Task(Fiber::ptr f, int thr)
            :fiber(f), thread(thr) {

        }
        
        Task(Fiber::ptr* f, int thr)
            :thread(thr) {
            fiber.swap(*f);
        }

        Task(std::function<void()> f, int thr)
            :cb(f), thread(thr) {
        }

        Task(std::function<void()>* f, int thr)
            :thread(thr) {
            cb.swap(*f);
        }

        Task()
            :thread(-1) {
        }

        void reset() {
            fiber = nullptr;
            cb = nullptr;
            thread = -1;
        }

    };

private:
    MutexType m_mutex;
    std::vector<Thread::ptr> m_threads;
    std::list<Task> m_tasks;
    Fiber::ptr m_rootFiber;
    std::string m_name;
protected:
    std::vector<int> m_threadIds;
    size_t m_threadCount = 0;
    std::atomic<size_t> m_activeThreadCount {0};
    std::atomic<size_t> m_idleThreadCount {0};
    bool m_stopping = true;
    bool m_autoStop = false;
    int m_rootThread = 0;
};

} // namespace wille
#endif
