/*
* Module: base
* Author: cloudyang@2015/04/02
* Copyright@cloudyang2015
*/

#include "base/PlatformThread.h"
#include <errno.h>
#include <sched.h>
#include <unistd.h>
#include "base/Time.h"
#include "base/ThreadLocal.h"

namespace base {

namespace {

ThreadLocalPointer<char> kCurrentThreadName;

struct ThreadParams {
    PlatformThread::Delegate* m_delegate;
    bool m_joinable;
};

void* ThreadFunc(void* params) 
{
    ThreadParams* threadParams = static_cast<ThreadParams*>(params);
    PlatformThread::Delegate *delegate = threadParams->m_delegate;

    delegate->threadMain();
    delete threadParams;

    return NULL;
}

bool createThread(size_t stackSize, 
            bool joinable, 
            PlatformThread::Delegate* delegate, 
            PlatformThreadHandle* threadHandle, 
            PlatformThread::ThreadPriority priority)
{
    bool success = false;

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    if (!joinable)
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    if (stackSize > 0)
        pthread_attr_setstacksize(&attr, stackSize);

    ThreadParams* params = new ThreadParams;
    params->m_delegate = delegate;
    params->m_joinable = joinable;

    success = !pthread_create(threadHandle, &attr, ThreadFunc, params);
    if (priority)
        PlatformThread::setThreadPriority(*threadHandle, priority);

    pthread_attr_destroy(&attr);
    if (!success)
        delete params;
    return success;
}

}

PlatformThreadId PlatformThread::currentId()
{
    return getpid();    
}

void PlatformThread::yieldCurrentThread()
{
    sched_yield();
}

void PlatformThread::sleep(int duration)
{
    struct timespec sleepTime, remaining;
    sleepTime.tv_sec = duration/Time::kMillisecondsPerSecond;
    sleepTime.tv_nsec = (duration%Time::kMillisecondsPerSecond) * 
        Time::kMicrosecondsPerMillisecond * Time::kNanosecondsPerMicrosecond;

    while (nanosleep(&sleepTime, &remaining) == -1 && errno == EINTR)
        sleepTime = remaining;
}

void PlatformThread::setName(const char* name) 
{
    
}
const char* PlatformThread::getName()
{
    return "not implementation";
}

bool PlatformThread::create(size_t stackSize, Delegate* delegate, PlatformThreadHandle* threadHandle)
{
    return createThread(stackSize, true, delegate, threadHandle, ThreadPriority_Normal);
}

bool PlatformThread::createWithPriority(size_t stackSize, Delegate* delegate, PlatformThreadHandle* threadHandle, ThreadPriority priority)
{
    return createThread(stackSize, true, delegate, threadHandle, priority);
}

bool PlatformThread::createNonJoinable(size_t stackSize, Delegate* delegate)
{
    PlatformThreadHandle unused;
    return createThread(stackSize, false, delegate, &unused, ThreadPriority_Normal);
}

void PlatformThread::join(PlatformThreadHandle threadHandle)
{
    // check thread can joinable.
    pthread_join(threadHandle, NULL);
}

void PlatformThread::setThreadPriority(PlatformThreadHandle threadHandle, ThreadPriority priority)
{
    // NotImplementation
}

}
