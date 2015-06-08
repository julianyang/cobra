/*
* Module: base
* Author: cloudyang@2015/04/02
* Copyright@cloudyang2015
*/

#include "base/WaitableEvent.h"
#include <math.h>
#include "base/Time.h"

namespace base {

WaitableEvent::WaitableEvent() 
    : m_handle(CreateEvent(NULL, false, false, NULL))
{
    
}

WaitableEvent::~WaitableEvent()
{
    CloseHandle(m_handle);
}

void WaitableEvent::wait()
{
    DWORD result = WaitForSingleObject(m_handle, INFINITE);
}

bool WaitableEvent::timedWait(int millisecond)
{
    double timeout = millisecond;
    DWORD result = WaitForSingleObject(m_handle, static_cast<DWORD>(timeout));
    switch (result) {
    case WAIT_OBJECT_O:
        return true;
    case WAIT_TIMEOUT:
        return false;
    default:
        break;
    }

    return false;
}

void WaitableEvent::notify()
{
    SetEvent(m_handle);
}

void WaitableEvent::notifyAll()
{
    ResetEvent(m_handle);
}

}
