/*
* Module: base
* Author: cloudyang@2015/04/02
* Copyright@cloudyang2015
*/

#include "base/ThreadLocal.h"
#include <windows.h>

namespace base {
namespace internal{

void ThreadLocalPlatform::allocateSlot(SlotType& slot)
{
    slot = TlsAlloc();
}

void ThreadLocalPlatform::freeSlot(SlotType& slot)
{
    TlsFree(slot);
}

void* ThreadLocalPlatform::getValueFromSlot(SlotType& slot)
{
    return TlsGetValue(slot);
}

void ThreadLocalPlatform::setValueInSlot(SlotType& slot, void* ptr)
{
    TlsSetValue(slot, ptr);
}

}
}
