/*
* Module: base
* Author: cloudyang@2015/03/28
* Copyright@cloudyang2015
*/

#include "base/AtomicRefCount.h"

namespace base {
namespace internal {

bool AtomicRefCount::hasOneRef() const
{
    int temp = m_refCount;
    __asm__ __volatile__("" : : : "memory");
    return temp == 1;
}

void AtomicRefCount::inc()
{
    int temp = 1;
    __asm__ __volatile__("lock; xaddl %0, %1"
                        : "+r" (temp), "+m" (m_refCount)
                        : : "memory");
}

bool AtomicRefCount::dec()
{
    int temp = -1;
    __asm__ __volatile__("lock; xaddl %0, %1"
                        : "+r" (temp), "+m" (m_refCount)
                        : : "memory");

    return (temp - 1) == 0;
}

int AtomicRefCount::refCount() const
{
    int temp = m_refCount;
    __asm__ __volatile__("" : : : "memory");
    return temp;
}

}
}
