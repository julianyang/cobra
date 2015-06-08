/*
* Module: base
* Author: cloudyang@2015/04/02
* Copyright@cloudyang2015
*/

#ifndef _BASE_TASK_H_
#define _BASE_TASK_H_

#include <queue>
#include "base/RefCountedThreadSafe.h"
#include "base/Time.h"
#include "base/RefPtr.h"

namespace base {

class TaskBase;

class PendingTask {
public:
    PendingTask(TaskBase* task, int delay = 0);

    void run() const;

    bool operator<(const PendingTask& o) const;

    RefPtr<TaskBase> m_task;

    int m_sequenceNum;
    Time m_timePosted;
    long long m_delayedRunTime;
};

class TaskQueue : public std::queue<PendingTask> {
public:
    void swap(TaskQueue* queue) { c.swap(queue->c); }
};

typedef std::priority_queue<PendingTask> DelayedTaskQueue;

class TaskBase : public RefCountedThreadSafe<TaskBase> {
public:
    TaskBase() 
        : m_canceled(false) 
        , m_interval(0)
    {
        
    }

    void cancel() { m_canceled = true; }
    bool canceled() const { return m_canceled;}
    void setInterval(int interval) { m_interval = interval; }
    int getInterval() const { return m_interval; }

    virtual void fired() = 0;

protected:
    friend class RefCountedThreadSafe<TaskBase>;
    virtual ~TaskBase() { }

    bool m_canceled;
    int m_interval;
};


template<typename T>
class Task : public TaskBase {
public:
    typedef void (T::*TaskFiredFunction)();

    Task(T* o, TaskFiredFunction f)
        : m_object(o)
        , m_function(f)
    {
        
    }

private:
    virtual void fired() { (m_object->*m_function)(); }

    T* m_object;
    TaskFiredFunction m_function;
};

}

#endif
