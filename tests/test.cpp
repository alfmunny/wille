#include <iostream>
#include <string>
#include "../wille/log.h"

int main() {
    wille::Logger::ptr logger(new wille::Logger);
    logger->addAppender(wille::LogAppender::ptr(new wille::StdoutLogAppender));
    std::string thread_name = "thread_one";

    wille::LogEvent::ptr event(new wille::LogEvent(logger, wille::LogLevel::DEBUG, __FILE__, __LINE__, 0, 1, 2, time(0), thread_name));
    event->getSS() << "wille sylar log";

    logger->log(wille::LogLevel::DEBUG, event);
    return 0;
}
