/*
* Module: base
* Author: cloudyang@2015/04/02
* Copyright@cloudyang2015
*/

#ifndef _BASE_RUNLOOPPUMP_H_
#define _BASE_RUNLOOPPUMP_H_

#include "base/OwnPtr.h"

namespace base {

class RunLoopPump {
public:
    class Delegate {
    public:
        virtual ~Delegate() { }
        virtual bool doTask() = 0;
        virtual bool doDelayedTask(long long* nextDelayedTaskTime) = 0;
        virtual bool doIdleTask() = 0;
    };


    RunLoopPump() { }

    virtual void run(Delegate* delegate) = 0;
    virtual void quit() = 0;
    virtual void scheduleTask() = 0;
    virtual void scheduleDelayedTask(const long long delayedTaskTime) = 0;
    
protected:
    friend void deleteOwnedPtr<RunLoopPump>(RunLoopPump*);
    virtual ~RunLoopPump() { }
};

}

#endif
