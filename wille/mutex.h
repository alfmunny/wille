#ifndef __WILLE_MUTEX_H__
#define __WILLE_MUTEX_H__

#include <pthread.h>
#include <semaphore.h>
#include <cstdint>

namespace wille {
class Semaphore {
public:
    Semaphore(uint32_t count = 0);
    ~Semaphore();

    void wait();
    void notify();

private:
    Semaphore(const Semaphore&) = delete;
    Semaphore(const Semaphore&&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;

private:
    sem_t* m_semaphore;
};

template <class T> class ScopedLockImpl {
public:
    ScopedLockImpl(T& mutex) : m_mutex(mutex) {
        m_mutex.lock();
        m_locked = true;
    }

    ~ScopedLockImpl() { unlock(); }

    void lock() {
        if (!m_locked) {
            m_mutex.lock();
            m_locked = true;
        }
    }

    void unlock() {
        if (m_locked) {
            m_mutex.unlock();
            m_locked = false;
        }
    }

private:
    T& m_mutex;
    bool m_locked;
};

template <class T> class ReadScopedLockImpl {
public:
    ReadScopedLockImpl(T& mutex) : m_mutex(mutex) {
        m_mutex.rdlock();
        m_locked = true;
    }

    ~ReadScopedLockImpl() { unlock(); }

    void lock() {
        if (!m_locked) {
            m_mutex.lock();
            m_locked = true;
        }
    }

    void unlock() {
        if (m_locked) {
            m_mutex.unlock();
            m_locked = false;
        }
    }

private:
    T& m_mutex;
    bool m_locked;
};

template <class T> class WriteScopedLockImpl {
public:
    WriteScopedLockImpl(T& mutex) : m_mutex(mutex) {
        m_mutex.wrlock();
        m_locked = true;
    }

    ~WriteScopedLockImpl() { unlock(); }

    void lock() {
        if (!m_locked) {
            m_mutex.lock();
            m_locked = true;
        }
    }

    void unlock() {
        if (m_locked) {
            m_mutex.unlock();
            m_locked = false;
        }
    }

private:
    T& m_mutex;
    bool m_locked;
};

class Mutex {
public:
    typedef ScopedLockImpl<Mutex> Lock;

    Mutex(const Mutex&) = delete;
    Mutex(const Mutex&&) = delete;
    Mutex& operator=(const Mutex&) = delete;

    Mutex() { pthread_mutex_init(&m_mutex, nullptr); }

    ~Mutex() { pthread_mutex_destroy(&m_mutex); }

    void lock() { pthread_mutex_lock(&m_mutex); }

    void unlock() { pthread_mutex_unlock(&m_mutex); }

private:
    pthread_mutex_t m_mutex;
};

class NullMutex {
public:
    typedef ScopedLockImpl<NullMutex> Lock;

    NullMutex(const NullMutex&) = delete;
    NullMutex(const NullMutex&&) = delete;
    NullMutex& operator=(const NullMutex&) = delete;

    NullMutex() {}

    ~NullMutex() {}

    void lock() {}

    void unlock() {}
};

class RWMutex {
public:
    RWMutex(const RWMutex&) = delete;
    RWMutex(const RWMutex&&) = delete;
    RWMutex& operator=(const RWMutex&) = delete;

    typedef ReadScopedLockImpl<RWMutex> ReadLock;
    typedef WriteScopedLockImpl<RWMutex> WriteLock;

    RWMutex() { pthread_rwlock_init(&m_lock, nullptr); }

    ~RWMutex() { pthread_rwlock_destroy(&m_lock); }

    void rdlock() { pthread_rwlock_rdlock(&m_lock); }

    void wrlock() { pthread_rwlock_wrlock(&m_lock); }

    void unlock() { pthread_rwlock_unlock(&m_lock); }

private:
    pthread_rwlock_t m_lock;
};

} // namespace wille

#endif
