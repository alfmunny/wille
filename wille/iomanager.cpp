#include "iomanager.h"
#include "log.h"
#include "macro.h"
#include <sys/event.h>
#include <sys/time.h>

namespace wille {

static Logger::ptr g_logger = WILLE_LOG_NAME("system");


IOManager::FdContext::EventContext& IOManager::FdContext::getContext(IOManager::Event event) {
    switch(event) {
        case IOManager::READ:
            return read;
        case IOManager::WRITE:
            return write;
        default:
            WILLE_ASSERT2(false, "getContext");
    }
}

void IOManager::FdContext::resetContext(EventContext& ctx) {
    ctx.scheduler = nullptr;
    ctx.fiber.reset() ;
    ctx.cb = nullptr;
}

void IOManager::FdContext::triggerEvent(IOManager::Event event) {
    WILLE_ASSERT(event == IOManager::READ || event == IOManager::WRITE);
    EventContext& ctx = getContext(event);
    if(ctx.cb) {
        ctx.scheduler->schedule(&ctx.cb);
    } else {
        ctx.scheduler->schedule(&ctx.fiber);
    }
    ctx.scheduler = nullptr;
}

IOManager::IOManager(size_t threads, bool use_caller, const std::string& name) 
    :Scheduler(threads, use_caller, name) {
    m_kqfd = kqueue();
    WILLE_ASSERT(m_kqfd != -1);

    int rt = pipe(m_tickleFds);
    WILLE_ASSERT(!rt);

    struct kevent event;
    EV_SET(&event, m_tickleFds[0], EVFILT_READ, EV_ADD, 0, 0, 0);

    contextResize(32);

    start();
}

IOManager::~IOManager() {
    stop();
    close(m_kqfd);
    close(m_tickleFds[0]);
    close(m_tickleFds[1]);

    for(size_t i = 0; i < m_fdContexts.size(); ++i) {
        if(m_fdContexts[i]) {
            delete m_fdContexts[i];
        }
    }
}

void IOManager::contextResize(size_t size) {
    m_fdContexts.resize(size);

    for (size_t i = 0; i < m_fdContexts.size(); ++i) {
        if(!m_fdContexts[i]) {
            m_fdContexts[i] = new FdContext;
            m_fdContexts[i]->fd = i;
        }
    }
}

bool IOManager::addEvent(int fd, Event event, std::function<void()> cb) {
    FdContext* fd_ctx = nullptr;
    RWMutexType::ReadLock lock(m_mutex);
    if((int)m_fdContexts.size() > fd) {
        fd_ctx = m_fdContexts[fd];
        lock.unlock();
    } else {
        lock.unlock();
        RWMutexType::WriteLock lock2(m_mutex);
        contextResize(fd * 1.5);
        fd_ctx = m_fdContexts[fd];
    }


    FdContext::MutexType::Lock lock2(fd_ctx->mutex);

    //if(fd_ctx->events & event) {
        //WILLE_LOG_ERROR(g_logger) << "addEvent assert fd=" << fd
            //<< " event=" << event
            //<< " fd_ctx.event=" << fd_ctx->events;
        //WILLE_ASSERT(!(fd_ctx->events & event));
    //}

    // int op = fd->events ? 
    struct kevent kev;
    if(event == READ) {
        EV_SET(&kev, fd, EVFILT_READ, EV_ADD, 0, 0, fd_ctx);
        fd_ctx->event_read = true;
    } else if (event == WRITE) {
        EV_SET(&kev, fd, EVFILT_WRITE, EV_ADD, 0, 0, fd_ctx);
        fd_ctx->event_write = true;
    } else {

    }

    ++m_pendingEventCount;

    FdContext::EventContext& event_ctx = fd_ctx->getContext(event);
    WILLE_ASSERT(!event_ctx.scheduler 
            && !event_ctx.fiber
            && !event_ctx.cb);

    event_ctx.scheduler = Scheduler::GetThis();

    if(cb) {
        event_ctx.cb.swap(cb);
    } else {
        event_ctx.fiber = Fiber::GetThis();
        WILLE_ASSERT(event_ctx.fiber->getState() == Fiber::EXEC);
    }

    return 0;
}

bool IOManager::delEvent(int fd, Event event) {
    RWMutexType::ReadLock lock(m_mutex);
    if(m_fdContexts.size() <= fd) {
        return false;
    }
    FdContext* fd_ctx = m_fdContexts[fd];
    lock.unlock();

    FdContext::MutexType::Lock lock2(fd_ctx->mutex);
    
    struct kevent kev;
    kev.udata = fd_ctx;
    if(fd_ctx->event_read) {
        EV_SET(&e)

    }

}

bool IOManager::cancelEvent(int fd, Event event) {

}

bool IOManager::cancelAll(int fd) {

}

IOManager* IOManager::GetThis() {

}

void IOManager::tickle() {

}

bool IOManager::stopping() {

}

void IOManager::idle() {

}

}
