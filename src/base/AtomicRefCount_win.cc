/*
* Module: base
* Author: cloudyang@2015/03/28
* Copyright@cloudyang2015
*/

#include "base/AtomicRefCount.h"
#include <windows.h>

namespace base {
namespace internal {

bool AtomicRefCount::hasOneRef()
{
    return m_refCount == 1;
}

void AtomicRefCount::inc()
{
    InterlockedExchangeAdd(reinterpret_cast<volatile long*>(&m_refCount), static_cast<long>(1));
}

bool AtomicRefCount::dec()
{
    return InterlockedExchangeAdd(reinterpret_cast<volatile long*>(&m_refCount), static_cast<long>(-1)) -1 == 0;
}

int AtomicRefCount::refCount()
{
    return m_refCount;
}

}
}
