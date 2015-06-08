/*
* Module: base
* Author: cloudyang@2015/04/02
* Copyright@cloudyang2015
*/

#include "base/RunLoopPumpImpl.h"
#include "base/Time.h"

namespace base {


RunLoopPumpImpl::RunLoopPumpImpl()
    : m_keepRunning(true)
    , m_event()
{
    
}

void RunLoopPumpImpl::run(RunLoopPump::Delegate* delegate)
{
    for (;;) {
        bool didTask = delegate->doTask();
        if (!m_keepRunning)
            break;

        didTask |= delegate->doDelayedTask(&m_delayedTaskTime);
        if (!m_keepRunning)
            break;

        if (didTask)
            continue;

        didTask |= delegate->doIdleTask();
        if (!m_keepRunning)
            break;

        if (didTask)
            continue;

        if (m_delayedTaskTime == 0)
            m_event.wait();
        else {
            long long delay = m_delayedTaskTime - Time::now().toInternalValue();
            if (delay > 0)
                m_event.timedWait(delay/Time::kMicrosecondsPerMillisecond);
            else
                m_delayedTaskTime = 0;
        }
    }
}

void RunLoopPumpImpl::quit()
{
    m_keepRunning = false;
}

void RunLoopPumpImpl::scheduleTask()
{
    m_event.notify();
}

void RunLoopPumpImpl::scheduleDelayedTask(const long long delayedTaskTime)
{
    m_delayedTaskTime = delayedTaskTime;
}

}
