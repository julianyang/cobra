/*
* Module: base
* Author: cloudyang@2015/04/02
* Copyright@cloudyang2015
*/

#ifndef _BASE_RUNLOOPPUMP_IMPL_H_
#define _BASE_RUNLOOPPUMP_IMPL_H_

#include "base/RunLoopPump.h"
#include "base/Noncopyable.h"
#include "base/WaitableEvent.h"

namespace base {

class RunLoopPumpImpl : public RunLoopPump {
    BASE_MAKE_NONCOPYABLE(RunLoopPumpImpl);
public:
    RunLoopPumpImpl();

    virtual void run(RunLoopPump::Delegate* delegate);
    virtual void quit();
    virtual void scheduleTask();
    virtual void scheduleDelayedTask(const long long delayedTaskTime);
    
protected:
    virtual ~RunLoopPumpImpl() { }

private:
    bool m_keepRunning;
    WaitableEvent m_event;
    long long m_delayedTaskTime;
};

}

#endif
