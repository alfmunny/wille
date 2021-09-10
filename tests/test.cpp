#include <iostream>
#include <string>
#include "../wille/log.h"

int main() {
    wille::Logger::ptr logger(new wille::Logger);
    logger->addAppender(wille::LogAppender::ptr(new wille::StdoutLogAppender));

    wille::FileLogAppender::ptr file_appender(new wille::FileLogAppender("./log.txt"));
    wille::LogFormatter::ptr fmt(new wille::LogFormatter("%d%T%p%T%m%n"));
    file_appender->setFormatter(fmt);
    file_appender->setLevel(wille::LogLevel::ERROR);

    logger->addAppender(file_appender);

    //wille::LogEvent::ptr event(new wille::LogEvent(logger, wille::LogLevel::DEBUG, __FILE__, __LINE__, 0, 1, 2, time(0), thread_name));
    //event->getSS() << "wille sylar log";
    WILLE_LOG_DEBUG(logger) << "wille log";
    WILLE_LOG_ERROR(logger) << "wille log error";
    return 0;
}
