#include "wille/wille.h"
#include "wille/iomanager.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>

wille::Logger::ptr g_logger = WILLE_LOG_ROOT();

int sock = 0;

void test_fiber() { WILLE_LOG_INFO(g_logger) << "test_fiber sock=" << sock;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sock, F_SETFL, O_NONBLOCK);

    sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "103.235.46.39", &addr.sin_addr.s_addr);

    if(!connect(sock, (const sockaddr*)&addr, sizeof(addr))) {
    } else if (errno == EINPROGRESS){
        WILLE_LOG_INFO(g_logger) << "add event errno=" << errno << " " << strerror(errno);

        wille::IOManager::GetThis()->addEvent(sock, wille::IOManager::READ, [](){
            WILLE_LOG_INFO(g_logger) << "read callback";
        });
        wille::IOManager::GetThis()->addEvent(sock, wille::IOManager::WRITE, [](){
            WILLE_LOG_INFO(g_logger) << "write callback";
            wille::IOManager::GetThis()->cancelEvent(sock, wille::IOManager::READ);
            close(sock);
        });

    } else {
        WILLE_LOG_INFO(g_logger) << "else" << errno << " " << strerror(errno);
    }
}

wille::Timer::ptr s_timer;
void test_timer() {
    wille::IOManager iom(2);
    s_timer = iom.addTimer(1000, [](){
        static int i = 0;
        WILLE_LOG_INFO(g_logger) << "hello timer i=" << i;
        if(++i == 3) {
            s_timer->reset(2000, true);
        }
    }, true);
}

void test1() {
    //std::cout << "EPOLLIN=" << EPOLLIN
    //   << "EPOLLOUT=" << EPOLLOUT << std::endl;
    wille::IOManager iom(2, false, "iom");
    //wille::IOManager iom;
    iom.schedule(&test_fiber);
}
int main() {
    //test1();
    test_timer();
    return 0;
}

