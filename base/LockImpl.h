/*
* Module: base
* Author: cloudyang@2015/03/31
* Copyright@cloudyang2015
*/

#ifndef _BASE_LOCK_IMPL_H_
#define _BASE_LOCK_IMPL_H_

#include "base/BuildConfig.h"
#include "base/Noncopyable.h"

#if defined(OS_WIN)
#include <windows.h>
#elif defined(OS_POSIX)
#include <pthread.h>
#endif

namespace base {
namespace internal {

class LockImpl {
    BASE_MAKE_NONCOPYABLE(LockImpl);
public:
#if defined(OS_WIN)
    typedef CRITICAL_SECTION OSLockType;
#elif defined(OS_POSIX)
    typedef pthread_mutex_t OSLockType;
#endif

    LockImpl();
    ~LockImpl();

    bool tryLock();
    void lock();
    void unlock();

    OSLockType* osLock() { return &m_osLock; }

private:
    OSLockType m_osLock;
};

}
}

#endif
