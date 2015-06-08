/*
* Module: base
* Author: cloudyang@2015/04/1
* Copyright@cloudyang2015
*/

#ifndef _BASE_TIME_H_
#define _BASE_TIME_H_

#include <time.h>
#include <limits>

namespace base {

class Time {
public:
    static const long long kMillisecondsPerSecond = 1000;
    static const long long kMicrosecondsPerMillisecond = 1000;
    static const long long kMicrosecondsPerSecond = kMillisecondsPerSecond * kMicrosecondsPerMillisecond;
    static const long long kMicrosecondsPerMinute = kMicrosecondsPerMillisecond * 60;
    static const long long kMicrosecondsPerHour = kMicrosecondsPerMinute * 60;
    static const long long kMicrosecondsPerDay = kMicrosecondsPerHour * 24;
    static const long long kMicrosecondsPerWeek = kMicrosecondsPerDay * 7;
    static const long long kNanosecondsPerMicrosecond = 1000;
    static const long long kNanosecondsPerSecond = kNanosecondsPerMicrosecond * kMicrosecondsPerSecond;
    static const long long kWindowsEpochDeltaSeconds;
    static const long long kWindowsEpochDeltaMicroseconds;

    struct Exploded {
        int m_year;
        int m_month;
        int m_dayOfWeek;
        int m_dayOfMonth;
        int m_hour;
        int m_minute;
        int m_second;
        int m_millisecond;
        
        bool hasValidValues() const;
    };

    explicit Time() : m_us(0) { }
    bool isNull() const { return m_us == 0; }
    bool isMax() const { return m_us == std::numeric_limits<long long>::max(); }

    static Time now();
    static Time nowFromSystemTime();
    static Time fromTimeT(time_t tt);
    time_t toTimeT() const;

    static Time fromUTCExploded(const Exploded& exploded) 
    {
        return fromExploded(false, exploded);
    }
    
    static Time fromLocalExploded(const Exploded& exploded) 
    {
        return fromExploded(true, exploded);
    }

    static Time fromInternalValue(long long us) { return Time(us); }
    long long toInternalValue() const { return m_us; }

    void UTCExploded(Exploded* exploded) const { return explodedInternal(false, exploded); }
    void localExploded(Exploded* exploded) const { return explodedInternal(true, exploded); }

    Time& operator=(Time& o) 
    {
        m_us = o.m_us;
        return *this;
    }

    bool operator==(Time& o) const { return m_us == o.m_us; }
    bool operator!=(Time& o) const { return m_us != o.m_us; }
    bool operator<(Time& o) const { return m_us < o.m_us; }
    bool operator<=(Time& o) const { return m_us <= o.m_us; }
    bool operator>(Time& o) const { return m_us > o.m_us; }
    bool operator>=(Time& o) const { return m_us >= o.m_us; }
    
private:
    explicit Time(long long us) : m_us(us) { }
    void explodedInternal(bool isLocal, Exploded* exploded) const;
    static Time fromExploded(bool isLocal, const Exploded& exploded);

    static const long long sTimeTToMicrosecondsOffset;
    long long m_us;
};

}

#endif
