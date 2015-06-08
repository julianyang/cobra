/*
* Module: base
* Author: cloudyang@2015/04/02
* Copyright@cloudyang2015
*/

#include "base/PlatformThread.h"
#include "base/Time.h"
#include "base/ThreadLocal.h"

namespace base {

namespace {

ThreadLocalPointer<char> kCurrentThreadName;

const DWORD kVCThreadNameException = 0x406D1388;

typedef struct tagTHREADNAME_INFO {
    DWORD dwType;
    LPCSTR szName;
    DWORD dwThreadID;
    DWORD dwFlags;
} THREADANME_INFO;

void setNameInternal(PlatformThreadId threadId, const char* name)
{
    THREADANME_INFO info;
    info.dwType = 0x1000;
    info.szName = name;
    info.dwThreadID = threadId;
    info.dwFlags = 0;

    __try {
        RaiseException(kVCThreadNameException, 0, sizeof(info)/sizeof(DWORD), reinterpret_cast<DWORD_PTR*>(&info));
    } __except(EXCEPTION_CONTINUE_EXECUTION) {
    }
}

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

bool createThreadInternal(size_t stackSize, 
            PlatformThread::Delegate* delegate, 
            PlatformThreadHandle* threadHandle) 
{
    PlatformThreadHandle handle;
    unsigned int flags = 0;

    if (stackSize > 0)
        flags = STACK_SIZE_PARAM_IS_A_RESERVATION;

    ThreadParams* params = new ThreadParams;
    params->m_delegate = delegate;
    params->m_joinable = threadHandle != NULL;

    handle = CreateThread(NULL, stackSize, ThreadFunc, params, flags, NULL);
    if (!handle) {
        delete params;
        return false;
    }

    if (threadHandle)
        *threadHandle = handle;
    else
        CloseHandle(handle);

    return true;
}

}

PlatformThreadId PlatformThread::currentId()
{
    return GetCurrentThreadId();
}

void PlatformThread::yieldCurrentThread()
{
    ::Sleep(0);
}

void PlatformThread::sleep(int duration)
{
    ::Sleep(duration);
}

void PlatformThread::setName(const char* name) 
{
    kCurrentThreadName.set(const_cast<char*>(name));

    setNameInternal(currentId(), name);
}

const char* PlatformThread::getName()
{
    return kCurrentThreadName.get();
}

bool PlatformThread::create(size_t stackSize, Delegate* delegate, PlatformThreadHandle* threadHandle)
{
    return createThreadInternal(stackSize, delegate, threadHandle);
}

bool PlatformThread::createWithPriority(size_t stackSize, Delegate* delegate, PlatformThreadHandle* threadHandle, ThreadPriority priority)
{
    bool result = create(stackSize, true, delegate, threadHandle, priority);
    if (result)
        setThreadPriority(*threadHandle, priority);

    return result;
}

bool PlatformThread::createNonJoinable(size_t stackSize, Delegate* delegate)
{
    return createThreadInternal(stackSize, delegate, NULL);
}

void PlatformThread::join(PlatformThreadHandle threadHandle)
{
    DWORD result = WaitForSingleObject(threadHandle, INFINITE);
    CloseHandle(threadHandle);
}

void PlatformThread::setThreadPriority(PlatformThreadHandle threadHandle, ThreadPriority priority)
{
    switch (priority) {
    case kThreadPriority_Normal:
        ::setThreadPriority(threadHandle, THREAD_PRIORITY_NORMAL);
        break;
    case kThreadPriority_RealtimeAudio:
        ::setThreadPriority(handle, THREAD_PRIORITY_TIME_CRITICAL);
        break;
    default:
        break;
    }
}

}
