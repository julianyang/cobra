/*
* Module: base
* Author: cloudyang@2015/04/02
* Copyright@cloudyang2015
*/

#include "base/Thread.h"
#include "base/WaitableEvent.h"
#include "base/RunLoop.h"

namespace base {

struct Thread::StartupData {
    const Thread::Options& m_options;
    WaitableEvent m_event;

    explicit StartupData(const Options& options)
        : m_options(options)
        , m_event()
    {
        
    }
}; 

Thread::Thread(const char* name) 
    : m_started(false)
    , m_stopping(false)
    , m_running(false)
    , m_startupData(NULL)
    , m_threadHandle(0)
    , m_threadId(kInvalidThreadId)
    , m_name(name)
    , m_runLoop(NULL)
{
    
}

Thread::~Thread()
{
    stop();
}

bool Thread::start()
{
    Options options;
    return startWithOptions(options);
}

bool Thread::startWithOptions(const Options& options)
{
    StartupData data(options);
    m_startupData = &data;

    if (!PlatformThread::create(options.m_stackSize, this, &m_threadHandle)) {
        m_startupData = NULL;
        return false;
    }

    data.m_event.wait();

    m_startupData = NULL;
    m_started = true;

    return true;
}

void Thread::stop()
{
    if (!m_started)
        return;

    stopSoon();
    PlatformThread::join(m_threadHandle);

    m_started = false;
    m_stopping = false;
}

void Thread::stopSoon()
{
    if (m_stopping || !m_runLoop)
        return;

    m_stopping = true;
    m_runLoop->postTask(new Task<Thread>(this, &Thread::threadQuitHelper));
}

bool Thread::isRunning() const
{
    return m_running;
}

void Thread::run(RunLoop* runLoop)
{
    runLoop->run();
} 

void Thread::threadMain()
{
    RunLoop runLoop;

    m_threadId = PlatformThread::currentId();
    PlatformThread::setName(m_name.c_str());
    m_runLoop = &runLoop;

    init();
    m_running = true;

    m_startupData->m_event.notify();

    run(m_runLoop);

    m_running = false;
    cleanUp();
    m_runLoop = NULL;
}

void Thread::threadQuitHelper()
{
    RunLoop::current()->quit();
}

}
