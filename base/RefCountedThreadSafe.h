/*
* Module: base
* Author: cloudyang@2015/03/28
* Copyright@cloudyang2015
*/

#ifndef _BASE_REFCOUNTED_THREADSAFE_H_
#define _BASE_REFCOUNTED_THREADSAFE_H_

//#include <atomic>
#include "base/Noncopyable.h"
#include "base/AtomicRefCount.h"

namespace base {

class ThreadSafeRefCountedBase {
public:
    void refBase() 
    {
        m_atomic.inc();
    }

    unsigned int refCount() const
    {
        return m_atomic.refCount();
    }

    bool hasOneRef() const
    {
        return m_atomic.hasOneRef();
    }

protected:
    ThreadSafeRefCountedBase()
        : m_atomic()
    {
        
    }

    ~ThreadSafeRefCountedBase() {}

    bool derefBase() 
    {
        return m_atomic.dec();    
    } 

private:
    internal::AtomicRefCount m_atomic;
};


template<typename T> 
class RefCountedThreadSafe : public ThreadSafeRefCountedBase {
    BASE_MAKE_NONCOPYABLE(RefCountedThreadSafe);
public:
    void ref() { refBase(); }
    void deref() 
    {
        if (derefBase())
            delete static_cast<T *>(this);
    }

protected:
    RefCountedThreadSafe() {}
    ~RefCountedThreadSafe() {}
};


}

#endif
