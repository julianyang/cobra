/*
* Module: base
* Author: cloudyang@2015/04/02
* Copyright@cloudyang2015
*/

#include "base/RunLoop.h"
#include <assert.h>
#include "base/ThreadLocal.h"

#include "base/RunLoopPumpImpl.h"

namespace base {

namespace {

ThreadLocalPointer<RunLoop> kRunLoopThreadLocalPointer;

}

#define FOR_EACH_OBSERVER(ObserverType, observerList, func) \
    do { \
        for (std::list<ObserverType*>::iterator it=observerList.begin(); it != observerList.end(); it++) { \
            ObserverType* obs = *it; \
            obs->func; \
        } \
    } while (0)

RunLoop::RunLoop()
    : m_quitWhenIdle(false)
    , m_nextSequenceNum(0)
    , m_pump(new RunLoopPumpImpl)
{
    kRunLoopThreadLocalPointer.set(this);
}

RunLoop::~RunLoop()
{
    assert(this == current());
    bool didTask;
    for (int i=0; i<100; i++) {
        deletePendingTasks();
        reloadTaskQueue();
        didTask = deletePendingTasks();
        if (!didTask)
            break;
    }
    
    FOR_EACH_OBSERVER(DestructionObserver, m_destructionObservers, willDestroyCurrentRunLoop());
}   

RunLoop* RunLoop::current()
{
    return kRunLoopThreadLocalPointer.get();
}

void RunLoop::addDestructionObserver(DestructionObserver* observer)
{
    assert(this == current());
    m_destructionObservers.push_back(observer);
}

void RunLoop::removeDestructionObserver(DestructionObserver* observer)
{
    m_destructionObservers.remove(observer);
}

void RunLoop::postTask(TaskBase* task) 
{
    PendingTask pendingTask(task);
    addToIncomingQueue(&pendingTask);
}

void RunLoop::postDelayedTask(TaskBase* task, int delay)
{
    PendingTask pendingTask(task, delay);
    addToIncomingQueue(&pendingTask);
}

void RunLoop::run()
{
    runInternal();    
}

void RunLoop::quitNow()
{
    assert(this == current());
    m_pump->quit();
}

void RunLoop::quitWhenIdle()
{
    assert(this == current());
    m_quitWhenIdle = true;
}

void RunLoop::addTaskObserver(TaskObserver* observer)
{
    assert(this == current());
    m_taskObservers.push_back(observer);
}

void RunLoop::removeTaskObserver(TaskObserver* observer)
{
    assert(this == current());
    m_taskObservers.remove(observer);
}

bool RunLoop::isRunning() const
{
    assert(this == current());
    return m_pump != NULL;
}

void RunLoop::runInternal()
{
    assert(this == current());   
    m_pump->run(this);
}

void RunLoop::runTask(const PendingTask& pendingTask)
{
    FOR_EACH_OBSERVER(TaskObserver, m_taskObservers, willProcessTask(pendingTask));
    pendingTask.run();
    FOR_EACH_OBSERVER(TaskObserver, m_taskObservers, didProcessTask(pendingTask));
}

void RunLoop::addToDelayedTaskQueue(const PendingTask& pendingTask)
{
    m_delayedTaskQueue.push(pendingTask);
}

void RunLoop::addToIncomingQueue(PendingTask* pendingTask)
{
    {
        AutoLock lock(m_incomingQueueLock);
        pendingTask->m_sequenceNum = m_nextSequenceNum++;
        bool wasEmpty = m_incomingTaskQueue.empty();
        m_incomingTaskQueue.push(*pendingTask);
        if (!wasEmpty)
            return;
    }

    m_pump->scheduleTask();
}

bool RunLoop::deletePendingTasks()
{
    bool didTask = !m_taskQueue.empty();

    while (!m_taskQueue.empty()) {
        PendingTask pendingTask = m_taskQueue.front();
        m_taskQueue.pop();
        if (pendingTask.m_delayedRunTime)
            addToDelayedTaskQueue(pendingTask);
    }

    didTask |= !m_delayedTaskQueue.empty();
    while (!m_delayedTaskQueue.empty())
        m_delayedTaskQueue.pop();

   return didTask;
}

void RunLoop::reloadTaskQueue()
{
    if (!m_taskQueue.empty())
        return;

    {
        AutoLock lock(m_incomingQueueLock);
        if (m_incomingTaskQueue.empty())
            return;

        m_incomingTaskQueue.swap(&m_taskQueue);
    }
}

bool RunLoop::doTask()
{
    for (;;) {
        reloadTaskQueue();
        if (m_taskQueue.empty())
            break;

        do {
            PendingTask pendingTask = m_taskQueue.front();
            m_taskQueue.pop();
            if (pendingTask.m_delayedRunTime) {
                addToDelayedTaskQueue(pendingTask);

                if (m_delayedTaskQueue.top().m_task == pendingTask.m_task)
                    m_pump->scheduleDelayedTask(pendingTask.m_delayedRunTime);
            } else {
                runTask(pendingTask);
                return true;
            }
        } while (!m_taskQueue.empty());
    }

    return false;
}

bool RunLoop::doDelayedTask(long long* nextDelayedTaskTime)
{
    if (m_delayedTaskQueue.empty()) {
        *nextDelayedTaskTime = 0;
        return false;
    }

    // FixMe: get a better view of dalayed time.
    if (Time::now().toInternalValue() < m_delayedTaskQueue.top().m_delayedRunTime) {
        *nextDelayedTaskTime = m_delayedTaskQueue.top().m_delayedRunTime;
        return false;
    }

    PendingTask pendingTask = m_delayedTaskQueue.top();
    m_delayedTaskQueue.pop();
    if (!m_delayedTaskQueue.empty())
        *nextDelayedTaskTime = m_delayedTaskQueue.top().m_delayedRunTime;
    
    runTask(pendingTask);
    return true;
}

bool RunLoop::doIdleTask()
{
    if (m_quitWhenIdle)
        m_pump->quit();

    return false;
}

}
