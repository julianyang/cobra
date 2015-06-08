#include <iostream>
#include "base/Task.h"
#include "base/RunLoop.h"
#include "base/Thread.h"
#include "base/Time.h"
#include "base/Logger.h"
#include "base/Singleton.h"

class ContextLogger : public base::Singleton<ContextLogger> {
public:
    ContextLogger() : m_logger() { }
    const base::Logger& logger() const { return m_logger; }

private:
    base::Logger m_logger;
};

#define DEBUGLOG ContextLogger::getInstance()->logger().debug()

class WorkerThread : public base::Thread {
public:
    WorkerThread(std::string &name)
        : Thread(name.c_str())
    {
    }

    ~WorkerThread()
    {
        stop();
    }

    virtual void cleanUp()
    {
        DEBUGLOG << "worker thread on destroy." << std::endl;
    }

    virtual void init()
    {
        DEBUGLOG << "worker thread on init." << std::endl;
    }
};

class Dispatcher {
public:
    void doWork();
    virtual void doWorkOnWorkerThread();
    void doDelayedWorkOnWorkerThread();
};

void Dispatcher::doWork()
{
    std::string name("wokerthread");
    WorkerThread childThread(name);
    base::Time::Exploded ex;
    base::Time::now().localExploded(&ex);

    childThread.start();
    DEBUGLOG << "create thread OK!" << std::endl;

    base::RefPtr<base::TaskBase> task = new base::Task<Dispatcher>(this, &Dispatcher::doWorkOnWorkerThread);
    base::RefPtr<base::TaskBase> delayedTask = new base::Task<Dispatcher>(this, &Dispatcher::doDelayedWorkOnWorkerThread);
    base::RunLoop* runLoop = childThread.runLoop();

    delayedTask->setInterval(100);
    runLoop->postDelayedTask(delayedTask.get(), 100);
    for (int i=0; i<10; i++) {
        runLoop->postTask(task.get());
        usleep(150000);
    }

    delayedTask->cancel();
}

void Dispatcher::doWorkOnWorkerThread()
{
    static int count = 0;
    DEBUGLOG << "do work " << ++count << " times!" << std::endl;
}

void Dispatcher::doDelayedWorkOnWorkerThread()
{
    DEBUGLOG << "do delayed work -----" << std::endl;
}

int main()
{
    base::Logger l;
    l.debug() << "-------begin-------" << std::endl;
    Dispatcher d;
    d.doWork();
    DEBUGLOG << "main thread will be quit!" << std::endl;
    l.debug() << "-------end-------" << std::endl;
    return 0;
}
