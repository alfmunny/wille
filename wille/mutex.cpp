#include "mutex.h"
#include <stdexcept>

namespace wille {
Semaphore::Semaphore(uint32_t count) : m_semaphore(nullptr) {
    m_semaphore = sem_open("semaphore", O_CREAT, S_IRUSR | S_IWUSR, 0, 0);
    if (!m_semaphore) {
        perror("sem_open");
        throw std::logic_error("sem_open error");
    }
}

Semaphore::~Semaphore() { sem_destroy(m_semaphore); }

void Semaphore::wait() {
    if (sem_wait(m_semaphore)) {
        perror("sem_wait");
        throw std::logic_error("sem_wait error");
    }
}

void Semaphore::notify() {
    if (sem_post(m_semaphore)) {
        perror("sem_post");
        throw std::logic_error("sem_post error");
    }
}
} // namespace wille
