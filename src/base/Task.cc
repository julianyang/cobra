/*
* Module: base
* Author: cloudyang@2015/04/02
* Copyright@cloudyang2015
*/

#include "base/Task.h"
#include "base/RunLoop.h"

namespace base {

PendingTask::PendingTask(TaskBase* task, int delay)
    : m_task(task)
    , m_sequenceNum(0)
    , m_timePosted(Time::now())
    , m_delayedRunTime(0LL)
{
    if (delay > 0)
        m_delayedRunTime = m_timePosted.toInternalValue() + delay * Time::kMicrosecondsPerMillisecond;
}

void PendingTask::run() const
{
    if (!m_task->canceled())
        m_task->fired();

    // check if it's a loop task.
    int delay = m_task->getInterval();
    if (delay > 0 && !m_task->canceled())
        RunLoop::current()->postDelayedTask(m_task.get(), delay); 
}

bool PendingTask::operator<(const PendingTask& o) const 
{
    if (m_delayedRunTime < o.m_delayedRunTime)
        return false;

    if (m_delayedRunTime > o.m_delayedRunTime)
        return true;
    
    return (m_sequenceNum - o.m_sequenceNum) > 0;
}

}
