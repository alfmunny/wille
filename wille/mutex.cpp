#include "mutex.h"
#include <string>

namespace wille {
Semaphore::Semaphore(uint32_t count) : m_semaphore(nullptr) {
    m_semaphore = sem_open("semephore", 0, O_CREAT, S_IRUSR | S_IWUSR, 0);
    if (!m_semaphore) {
        throw std::logic_error("sem_init error");
    }
}

Semaphore::~Semaphore() { sem_close(m_semaphore); }

void Semaphore::wait() {
    if (sem_wait(m_semaphore)) {
        throw std::logic_error("sem_wait error");
    }
}

void Semaphore::notify() {
    if (sem_post(m_semaphore)) {
        throw std::logic_error("sem_post error");
    }
}

} // namespace wille
