/*
* Module: base
* Author: cloudyang@2015/04/02
* Copyright@cloudyang2015
*/

#ifndef _BASE_PLATFORM_THREAD_H_
#define _BASE_PLATFORM_THREAD_H_

#include "base/Noncopyable.h"
#include "base/BuildConfig.h"

#if defined(OS_WIN)
#include <windows.h>
#elif defined(OS_POSIX)
#include <pthread.h>
#include <unistd.h>
#endif

namespace base {

#if defined(OS_WIN)
typedef DWORD PlatformThreadId;
typedef void* PlatformThreadHandle;
const PlatformThreadHandle kNullThreadHandle = NULL;
#elif defined(OS_POSIX)
typedef pid_t PlatformThreadId;
typedef pthread_t PlatformThreadHandle;
const PlatformThreadHandle kNullThreadHandle = NULL;
#endif

const PlatformThreadId kInvalidThreadId = 0;

class PlatformThread {
    BASE_MAKE_NONCOPYABLE(PlatformThread);
public:
    class Delegate {
    public:
        virtual void threadMain() = 0;

    protected:
        virtual ~Delegate() { }
    };  

    enum ThreadPriority {
        ThreadPriority_Normal,
        ThreadPriority_RealTimeAudio
    };

    static PlatformThreadId currentId();
    static void yieldCurrentThread();
    static void sleep(int duration);
    static void setName(const char* name);
    static const char* getName();

    static bool create(size_t stackSize, Delegate* delegate, PlatformThreadHandle* threadHandle);
    static bool createWithPriority(size_t stackSize, Delegate* delegate, PlatformThreadHandle* threadHandle, ThreadPriority priority);
    static bool createNonJoinable(size_t stackSize, Delegate* delegate);

    static void join(PlatformThreadHandle threadHandle);
    static void setThreadPriority(PlatformThreadHandle threadHandle, ThreadPriority priority);
};

}

#endif
