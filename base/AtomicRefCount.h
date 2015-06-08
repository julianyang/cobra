/*
* Module: base
* Author: cloudyang@2015/03/28
* Copyright@cloudyang2015
*/

#ifndef _BASE_ATOMIC_REFCOUNT_H_
#define _BASE_ATOMIC_REFCOUNT_H_

namespace base {
namespace internal {

class AtomicRefCount {
public:
    AtomicRefCount() : m_refCount(0) { }
    ~AtomicRefCount() { }

    bool hasOneRef() const;
    void inc();
    bool dec();
    int refCount() const;

private:
    volatile int m_refCount;
};

}
}

#endif
