/*
* Module: base
* Author: cloudyang@2015/04/02
* Copyright@cloudyang2015
*/

#include "base/WaitableEvent.h"
#include <errno.h>
#include "base/Time.h"

namespace base {

namespace {

struct timespec abstime(int millisecond)
{
    long long fireTime = Time::now().toInternalValue() + millisecond * Time::kMicrosecondsPerMillisecond;
    struct timespec sp;
    sp.tv_sec = fireTime/(Time::kMicrosecondsPerMillisecond * Time::kMillisecondsPerSecond);
    sp.tv_nsec = (fireTime % (Time::kMicrosecondsPerMillisecond * Time::kMillisecondsPerSecond))* Time::kNanosecondsPerMicrosecond;

    return sp;
}

}

WaitableEvent::WaitableEvent() 
    : m_lock()
{
    pthread_cond_init(&m_cond, NULL);    
}

WaitableEvent::~WaitableEvent()
{
    pthread_cond_destroy(&m_cond);
}

void WaitableEvent::wait()
{
    AutoLock lock(m_lock);
    pthread_cond_wait(&m_cond, m_lock.osLock());
}

bool WaitableEvent::timedWait(int millisecond)
{
    AutoLock lock(m_lock);
    struct timespec sp = abstime(millisecond);
    int rc = pthread_cond_timedwait(&m_cond, m_lock.osLock(), &sp);
    if (rc !=0 && rc != ETIMEDOUT)
        return false;
    return true;
}

void WaitableEvent::notify()
{
    AutoLock lock(m_lock);
    pthread_cond_signal(&m_cond);
}

void WaitableEvent::notifyAll()
{
    AutoLock lock(m_lock);
    pthread_cond_broadcast(&m_cond);
}

void WaitableEvent::waitWithoutLock()
{
    pthread_cond_wait(&m_cond, m_lock.osLock());
}

bool WaitableEvent::timedWaitWithoutLock(int millisecond)
{
    struct timespec sp = abstime(millisecond);
    int rc = pthread_cond_timedwait(&m_cond, m_lock.osLock(), &sp);
    if (rc !=0 && rc != ETIMEDOUT)
        return false;
    return true;
}

void WaitableEvent::notifyWithoutLock()
{
    pthread_cond_signal(&m_cond);
}

void WaitableEvent::notifyAllWithoutLock()
{
    pthread_cond_broadcast(&m_cond);
}

}
