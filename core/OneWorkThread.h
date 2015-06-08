/* 
 * Moudule: core
 * Author: cloudyang@2015/04/17
 */

#ifndef COBRA_CORE_ONEWORK_THREAD
#define COBRA_CORE_ONEWORK_THREAD

#include <string>
#include "base/Thread.h"

namespace CobraCore {



class OneWorkThread : public base::Thread {
public:

    class OneWorkThreadDelegate {
    public:
        virtual void beforeRun() { }
        virtual void afterRun() { }

    protected:
        virtual ~OneWorkThreadDelegate() {}
    };

    OneWorkThread(const char* name, OneWorkThreadDelegate* delgate) : base::Thread(name), m_delegate(delgate) { }
    ~OneWorkThread() { stop(); }
    
    virtual void init() { if (m_delegate) m_delegate->beforeRun(); }
    virtual void cleanup() { if (m_delegate) m_delegate->afterRun(); }

private:
    OneWorkThreadDelegate* m_delegate;
};

}

#endif
