#ifndef __WILLE_THREAD_H__
#define __WILLE_THREAD_H__

#include <sys/types.h>
#include <functional>
#include <memory>
#include <pthread.h>
#include <semaphore.h>
#include "mutex.h"
#include <string>
#include <thread>

namespace wille {

class Thread {
public:
    typedef std::shared_ptr<Thread> ptr;
    Thread(std::function<void()> cb, const std::string& name);
    ~Thread();

    pid_t getId() const { return m_id; }
    const std::string& getName() const { return m_name; }

    void join();

    static Thread* GetThis();
    static const std::string& GetName();
    static void SetName(const std::string& name);

private:
    Thread(const Thread&) = delete;
    Thread(const Thread&&) = delete;
    Thread& operator=(const Thread&) = delete;

    static void* run(void* arg);

private:
    uint64_t m_id = -1;
    pthread_t m_thread = 0;
    std::function<void()> m_cb;
    std::string m_name;
    Semaphore m_semaphore;
   // boost::interprocess::interprocess_semaphore m_semaphore;
};

} // namespace wille

#endif
