/*
* Module: base
* Author: cloudyang@2015/04/02
* Copyright@cloudyang2015
*/

#ifndef _BASE_THREAD_H_
#define _BASE_THREAD_H_

#include <string>
#include "base/PlatformThread.h"
#include "base/Noncopyable.h"

namespace base {

class RunLoop;

class Thread : public PlatformThread::Delegate {
    BASE_MAKE_NONCOPYABLE(Thread);
public:
    struct Options {
        size_t m_stackSize;

        Options() : m_stackSize(0) { }
        Options(size_t stackSize) : m_stackSize(stackSize) { }
    };

    explicit Thread(const char* name);

    virtual ~Thread();

    bool start();
    bool startWithOptions(const Options& options);
    void stop();
    void stopSoon();
    
    RunLoop* runLoop() const { return m_runLoop; }
    const std::string& threadName() const { return m_name; }

    PlatformThreadHandle threadHandle() const { return m_threadHandle; }
    PlatformThreadId threadId() const { return m_threadId; }

    bool isRunning() const;
    void threadQuitHelper();

protected:
    virtual void init() { }
    virtual void run(RunLoop* runLoop);
    virtual void cleanUp() { }

    void setRunLoop(RunLoop* runLoop) { m_runLoop = runLoop; }

private:
    virtual void threadMain();
    bool m_started;
    bool m_stopping;
    bool m_running;

    struct StartupData;
    StartupData* m_startupData;
    PlatformThreadHandle m_threadHandle;
    PlatformThreadId m_threadId;
    std::string m_name;
    RunLoop* m_runLoop;
}; 

}

#endif
