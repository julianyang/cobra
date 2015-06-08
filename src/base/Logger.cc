/*
* Module: base
* Author: cloudyang@2015/04/02
* Copyright@cloudyang2015
*/

#include "base/Logger.h"
#include <stdio.h>
#include <string.h>
#include "base/Time.h"

namespace base {

LoggerStream Logger::getOutputStream(int level) const
{
    bool isOut = false;
    std::string header;
    if (level <= m_level) {
        char head[120] = { 0 };
        constructHeader(head, level);
        header = head;
        isOut = true;
    }

    return LoggerStream(isOut, header);
}

void Logger::constructHeader(char* head, int level) const
{
    Time::Exploded exploded;
    Time::now().localExploded(&exploded);

    sprintf(head, "[%04d-%02d-%02d %02d:%02d:%02d]%d|", exploded.m_year, exploded.m_month, exploded.m_dayOfMonth,
        exploded.m_hour, exploded.m_minute, exploded.m_second, exploded.m_millisecond);

    int length = strlen(head);
    switch (level) {
    case E_LOG_ERROR:
        sprintf(head+length, "%s|", "error");
        break;
    case E_LOG_WARN:
        sprintf(head+length, "%s|", "warn");
        break;
    case E_LOG_DEBUG:
        sprintf(head+length, "%s|", "debug");
        break;
    case E_LOG_INFO:
        sprintf(head+length, "%s|", "info");
        break;
    default:
        break;
    }
}

}
