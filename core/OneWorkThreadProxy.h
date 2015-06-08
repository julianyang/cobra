/* 
 * Moudule: core
 * Author: cloudyang@2015/04/17
 */

#ifndef COBRA_CORE_ONEWORK_THREADPROXY
#define COBRA_CORE_ONEWORK_THREADPROXY

#include <string>
#include "base/RefPtr.h"
#include "base/Task.h"
#include "core/OneWorkThread.h"

namespace CobraCore {

class OneWorkThreadProxy : public OneWorkThread::OneWorkThreadDelegate {
protected:

    OneWorkThreadProxy(const std::string& name, int ms = 100) 
        : m_workThread(name.c_str(), this)
        , m_interval(ms) 
        , m_running(false)
    {
    }

    virtual ~OneWorkThreadProxy();


public:
    void start();
    void stop(); 

    void setWorkInterval(int interval /*ms*/);
    int getWorkInterval() const { return m_interval; }
    
    bool isRunning() const { return m_running; }

    virtual void beforeRun() { }
    virtual void doWork() = 0;
    virtual void afterRun() { }

private:
    OneWorkThread m_workThread;
    int m_interval;
    bool m_running;
    base::RefPtr<base::TaskBase> m_task;
};

}

#endif
