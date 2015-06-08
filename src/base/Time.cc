/*
* Module: base
* Author: cloudyang@2015/04/01
* Copyright@cloudyang2015
*/

#include "base/Time.h"

namespace base {

const long long Time::kWindowsEpochDeltaSeconds = 11644473600LL;
const long long Time::kWindowsEpochDeltaMicroseconds = kWindowsEpochDeltaSeconds * Time::kMicrosecondsPerSecond;
const long long Time::sTimeTToMicrosecondsOffset = kWindowsEpochDeltaMicroseconds;

inline bool isInRange(int value, int low, int high)
{
    return low <= value && value <= high;
}

Time Time::fromTimeT(time_t tt)
{
    if (tt == 0)
        return Time();

    if (tt == std::numeric_limits<time_t>::max())
        return Time(std::numeric_limits<long long>::max());

    return Time((tt * kMicrosecondsPerSecond) + sTimeTToMicrosecondsOffset);
}

time_t Time::toTimeT() const 
{
    if (isNull())
        return 0;

    if (isMax())
        return std::numeric_limits<time_t>::max();

    if (std::numeric_limits<long long>::max() - sTimeTToMicrosecondsOffset <= m_us)
        return std::numeric_limits<time_t>::max();

    return (m_us - sTimeTToMicrosecondsOffset) / kMicrosecondsPerSecond;
}

bool Time::Exploded::hasValidValues() const 
{
    return isInRange(m_month, 1, 12)
        && isInRange(m_dayOfWeek, 0, 6)
        && isInRange(m_dayOfMonth, 1, 31)
        && isInRange(m_hour, 0, 23)
        && isInRange(m_minute, 0, 59)
        && isInRange(m_second, 0, 59)
        && isInRange(m_millisecond, 0, 999);
}

}
