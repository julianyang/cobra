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
    pthread_mutex_init(&m_osLock, NULL);
}

LockImpl::~LockImpl()
{
    pthread_mutex_destroy(&m_osLock);
}

bool LockImpl::tryLock()
{
    return pthread_mutex_trylock(&m_osLock) == 0;
}

void LockImpl::lock()
{
    pthread_mutex_lock(&m_osLock);
}

void LockImpl::unlock()
{
    pthread_mutex_unlock(&m_osLock);
}

}
}
