/*
* Module: base
* Author: cloudyang@2015/04/02
* Copyright@cloudyang2015
*/

#ifndef _BASE_RUNLOOP_H_
#define _BASE_RUNLOOP_H_

#include <list>
#include "base/Noncopyable.h"
#include "base/OwnPtr.h"
#include "base/Task.h"
#include "base/Lock.h"
#include "base/RunLoopPump.h"

namespace base {

class TaskBase;
class PendingTask;

class RunLoop : public RunLoopPump::Delegate {
    BASE_MAKE_NONCOPYABLE(RunLoop);
public:
    RunLoop();
    ~RunLoop();

    class DestructionObserver {
    public:
        virtual void willDestroyCurrentRunLoop() = 0;

    protected:
        virtual ~DestructionObserver();
    };

    void addDestructionObserver(DestructionObserver* observer);
    void removeDestructionObserver(DestructionObserver* observer);

    static RunLoop* current();

    void postTask(TaskBase* task);
    void postDelayedTask(TaskBase* task, int delay);

    void run();
    void quit() { quitWhenIdle(); }
    void quitWhenIdle();
    void quitNow();

    class TaskObserver {
    public:
        TaskObserver();

        virtual void willProcessTask(const PendingTask& pendingTask) = 0;
        virtual void didProcessTask(const PendingTask& pendingTask) = 0;

    protected:
        virtual ~TaskObserver() { }
    };

    void addTaskObserver(TaskObserver* observer);
    void removeTaskObserver(TaskObserver* observer);

    bool isRunning() const;

protected:
    void runInternal();

    void runTask(const PendingTask& pendingTask);
    void addToDelayedTaskQueue(const PendingTask& pendingTask);
    void addToIncomingQueue(PendingTask* pendingTask);
    bool deletePendingTasks();
    void reloadTaskQueue();

    virtual bool doTask();
    virtual bool doDelayedTask(long long* nextDelayedTaskTime);
    virtual bool doIdleTask();

    bool m_quitWhenIdle;
    int m_nextSequenceNum;
    mutable Lock m_incomingQueueLock;

    TaskQueue m_incomingTaskQueue;
    TaskQueue m_taskQueue;
    DelayedTaskQueue m_delayedTaskQueue;
    OwnPtr<RunLoopPump> m_pump;

    std::list<DestructionObserver*> m_destructionObservers;
    std::list<TaskObserver*> m_taskObservers;
};

}

#endif
