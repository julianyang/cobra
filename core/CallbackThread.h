/* 
 * Moudule: core
 * Author: cloudyang@2015/04/17
 */

#ifndef COBRA_CORE_CALLBACK_THREAD
#define COBRA_CORE_CALLBACk_THREAD

#include <string>
#include "base/Thread.h"
#include "base/RefPtr.h"

namespace CobraCore {

class RequestCallback;

class CallbackThread : public base::Thread {
public:

    CallbackThread() : base::Thread("CallbackThread") {}
    ~CallbackThread() { stop(); }
    
    virtual void init() {}
    virtual void cleanup() {}

    void doResponse(base::RefPtr<RequestCallback> cb, int code, const std::string& buffer);

};

}

#endif
