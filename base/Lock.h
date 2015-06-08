/*
* Module: base
* Author: cloudyang@2015/03/31
* Copyright@cloudyang2015
*/

#ifndef _BASE_LOCK_H_
#define _BASE_LOCK_H_

#include "base/Noncopyable.h"
#include "base/LockImpl.h"

namespace base {

class Lock {
    BASE_MAKE_NONCOPYABLE(Lock);
public:
    Lock() : m_lock() { }
    ~Lock() { }

    void acquire() { m_lock.lock(); }
    void release() { m_lock.unlock(); }
    bool tryAcquire() { return m_lock.tryLock(); }
    internal::LockImpl::OSLockType* osLock() { return m_lock.osLock(); }    

private:
    internal::LockImpl m_lock;   
};

class AutoLock {
    BASE_MAKE_NONCOPYABLE(AutoLock);
public:
    AutoLock(Lock& lock) 
        : m_lock(lock) 
    {
        m_lock.acquire();
    }

    ~AutoLock() 
    {
        m_lock.release();
    }

private:
    Lock& m_lock;
};

class AutoUnlock {
    BASE_MAKE_NONCOPYABLE(AutoUnlock);
public:
    AutoUnlock(Lock& lock)
        : m_lock(lock)
    {
        m_lock.release();
    }

    ~AutoUnlock()
    {
        m_lock.acquire();
    }

private:
    Lock& m_lock;
};

}

#endif
