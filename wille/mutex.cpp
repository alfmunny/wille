#include "mutex.h"
#include <stdexcept>

namespace wille {
Semaphore::Semaphore(uint32_t count) {
#ifdef __APPLE__
    m_semaphore = sem_open("semaphore", O_CREAT, S_IRUSR | S_IWUSR, 0, 0);
    if (!m_semaphore) {
        perror("sem_open");
        throw std::logic_error("sem_open error");
    }
#elif __linux
    if(sem_init(&m_semaphore, 0, count)) {
        throw std::logic_error("sem_open error");
    }
#endif
}

Semaphore::~Semaphore() { 
#ifdef __APPLE__
    sem_destroy(m_semaphore); 
#elif __linux
    sem_destroy(&m_semaphore); 
#endif
}

void Semaphore::wait() {
#ifdef __APPLE__
    if (sem_wait(m_semaphore)) {
#elif __linux
    if (sem_wait(&m_semaphore)) {
#endif
        perror("sem_wait");
        throw std::logic_error("sem_wait error");
    }
}

void Semaphore::notify() {
#ifdef __APPLE__
    if (sem_post(m_semaphore)) {
#elif __linux
    if (sem_post(&m_semaphore)) {
#endif
        perror("sem_post");
        throw std::logic_error("sem_post error");
    }
}
} // namespace wille
