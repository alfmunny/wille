#ifndef __WILLE_FIBER_H__
#define __WILLE_FIBER_H__
#define _XOPEN_SOURCE 600
#include <memory>
#include <functional>
#include <ucontext.h>

namespace wille {

class Fiber : public std::enable_shared_from_this<Fiber> {

public:
    typedef std::shared_ptr<Fiber> ptr;
    enum State {
        INIT,
        HOLD,
        EXEC,
        TERM,
        READY,
        EXCEPT
    };

private:
    Fiber();

public:
    Fiber(std::function<void()> cb, size_t stacksize = 0);
    ~Fiber();

    void reset(std::function<void()> cb);
    void swapIn();
    void swapOut();

    uint64_t getId() const { return m_id; }

public:
    static void SetThis(Fiber* f);
    static Fiber::ptr GetThis();
    static void YieldToReady();
    static void YieldToHold();
    static uint64_t TotalFibers();
    static uint64_t GetFiberId();
    static void MainFunc();
private:
    uint64_t m_id = 0;
    //uint32_t m_stacksize = 0;
    State m_state = INIT;

    ucontext_t m_ctx;
    char m_stack[1024*1024];

    std::function<void()> m_cb;
};

} // namespace wille
#endif
