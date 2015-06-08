/*
* Module: base
* Author: cloudyang@2015/04/02
* Copyright@cloudyang2015
*/

#include "base/ThreadLocal.h"

namespace base {
namespace internal{

void ThreadLocalPlatform::allocateSlot(SlotType& slot)
{
    pthread_key_create(&slot, NULL);
}

void ThreadLocalPlatform::freeSlot(SlotType& slot)
{
    pthread_key_delete(slot);
}

void* ThreadLocalPlatform::getValueFromSlot(SlotType& slot)
{
    return pthread_getspecific(slot);
}

void ThreadLocalPlatform::setValueInSlot(SlotType& slot, void* ptr)
{
    pthread_setspecific(slot, ptr);
}

}
}
