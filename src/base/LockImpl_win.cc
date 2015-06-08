/*
* Module: base
* Author: cloudyang@2015/03/31
* Copyright@cloudyang2015
*/

#include "base/LockImpl.h"

namespace base {
namespace internal {

LockImpl::LockImpl()
{
    ::InitializeCriticalSectionAndSpinCount(&m_osLock, 2000);
}

LockImpl::~LockImpl()
{
    ::DeleteCriticalSection(&m_osLock);
}

bool LockImpl::tryLock()
{
    if (::TryEnterCriticalSection(&m_osLcok) != FALSE)
        return true;
    return false;
}

void LockImpl::lock()
{
    ::EnterCriticalSection(&m_osLock);
}

void LockImpl::lock()
{
    ::LeaveCriticalSection(&m_osLock);
}

}
}
