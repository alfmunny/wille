#include <iostream>
#include <string>
#include "../wille/log.h"
#include "../wille/util.h"

static wille::Logger::ptr g_logger = WILLE_LOG_ROOT();

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
    WILLE_LOG_FMT_ERROR(logger, "test macro fmt error %s", "aa");

    WILLE_LOG_DEBUG(g_logger) << "wille static logger";
    return 0;
}
