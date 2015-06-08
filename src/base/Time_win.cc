/*
* Module: base
* Author: cloudyang@2015/04/01
* Copyright@cloudyang2015
*/

#include "base/Time.h"
#include <sys/time.h>
#include <unistd.h>

namespace base {

namespace {

void systemTimeToTimeStruct(time_t t, struct tm* timestruct, bool isLocal)
{
    if (isLocal)
        localtime_r(&t, timestruct);
    else 
        gmtime_r(&t, timestruct);
}

time_t systemTimeFromTimeStruct(struct tm* timestruct, bool isLocal)
{
    if (isLocal)
        return mktime(timestruct);
    else
        return timegm(timestruct);
}

}

Time Time::now() 
{
    struct timeval tv;
    struct timezone tz = { 0, 0 };

    if (gettimeofday(&tv, &tz) != 0)
        return Time();

    return Time((tv.tv_sec * kMicrosecondsPerSecond + tv.tv_usec) + Time::kWindowsEpochDeltaMicroseconds);
}

Time Time::nowFromSystemTime()
{
    return now();
}

void Time::explodedInternal(bool isLocal, Exploded* exploded) const
{
    long long microseconds = m_us - kWindowsEpochDeltaMicroseconds;
    long long milliseconds;

    time_t seconds;
    int millisecond;

    if (microseconds >= 0) {
        milliseconds = microseconds / kMicrosecondsPerMillisecond;
        seconds = milliseconds / kMillisecondsPerSecond;
        millisecond = milliseconds % kMillisecondsPerSecond;
    } else {
        milliseconds = (microseconds - kMicrosecondsPerMillisecond + 1) / kMicrosecondsPerMillisecond;
        seconds = (milliseconds - kMillisecondsPerSecond + 1) / kMillisecondsPerSecond;
        millisecond = milliseconds % kMillisecondsPerSecond;

        if (millisecond < 0)
            millisecond += kMillisecondsPerSecond;
    }

    struct tm timestruct;
    systemTimeToTimeStruct(seconds, &timestruct, isLocal);

    exploded->m_year = timestruct.tm_year + 1900;
    exploded->m_month = timestruct.tm_mon + 1;
    exploded->m_dayOfWeek = timestruct.tm_wday;
    exploded->m_dayOfMonth = timestruct.tm_mday;
    exploded->m_hour = timestruct.tm_hour;
    exploded->m_minute = timestruct.tm_min;
    exploded->m_second = timestruct.tm_sec;
    exploded->m_millisecond = millisecond;
}

Time Time::fromExploded(bool isLocal, const Exploded& exploded)
{
    struct tm timestruct;
    timestruct.tm_year = exploded.m_year - 1900;
    timestruct.tm_mon = exploded.m_month -1;
    timestruct.tm_wday = exploded.m_dayOfWeek;
    timestruct.tm_mday = exploded.m_dayOfMonth;
    timestruct.tm_hour = exploded.m_hour;
    timestruct.tm_min = exploded.m_minute;
    timestruct.tm_sec = exploded.m_second;
    timestruct.tm_yday = 0;
    timestruct.tm_isdst = -1;
    timestruct.tm_gmtoff = 0;
    timestruct.tm_zone = NULL;

    time_t seconds = systemTimeFromTimeStruct(&timestruct, isLocal);

    long long milliseconds;
    if (seconds == -1 && (exploded.m_year < 1969 || exploded.m_year > 1970)){
        if (exploded.m_year < 1969) 
            milliseconds = std::numeric_limits<time_t>::min() * kMillisecondsPerSecond;
        else 
            milliseconds = (std::numeric_limits<time_t>::max() * kMillisecondsPerSecond) + kMillisecondsPerSecond - 1;
    } else {
        milliseconds = seconds * kMillisecondsPerSecond + exploded.m_millisecond;
    }

    return Time((milliseconds * kMicrosecondsPerMillisecond) + kWindowsEpochDeltaMicroseconds);
}

}
