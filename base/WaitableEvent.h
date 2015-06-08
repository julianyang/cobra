/*
* Module: base
* Author: cloudyang@2015/04/02
* Copyright@cloudyang2015
*/

#ifndef _BASE_WAITABLE_EVENT_H_
#define _BASE_WAITABLE_EVENT_H_

#include "base/BuildConfig.h"
#include "base/Noncopyable.h"
#include "base/Lock.h"

#if defined(OS_WIN)
#include <windows.h>
#elif defined(OS_POSIX)
#include <pthread.h>
#include <unistd.h>
#endif

namespace base {

class WaitableEvent {
    BASE_MAKE_NONCOPYABLE(WaitableEvent);
public:
    WaitableEvent();
    ~WaitableEvent();

    void wait();
    bool timedWait(int millisecond);
    void notify();
    void notifyAll();

    void waitWithoutLock();
    bool timedWaitWithoutLock(int millisecond);
    void notifyWithoutLock();
    void notifyAllWithoutLock();
    const Lock& getLock() const { return m_lock; }

private:
#if defined(OS_WIN)
    HANDLE m_handle;
#elif defined(OS_POSIX)
    Lock m_lock;
    pthread_cond_t m_cond;
#endif
};

}

#endif
