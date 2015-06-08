/* 
 * Moudule: core
 * Author: cloudyang@2015/04/17
 */

#include "core/CallbackThread.h"

#include "base/Task.h"
#include "base/RunLoop.h"
#include "core/RequestCallback.h"
#include "core/CoreLogger.h"

namespace CobraCore {

class CallbackTask {
public:
    explicit CallbackTask(base::RefPtr<RequestCallback> cb, int code, const std::string& buffer)
        : m_cb(cb)
        , m_code(code)
        , m_buffer(buffer)
    {
        CORE_DEBUG << "CallbackTask create" << std::endl;
    }

    ~CallbackTask() { CORE_DEBUG << "CallbackTask destroy" << std::endl; }

    void doResponse();

private:
    base::RefPtr<RequestCallback> m_cb;
    int m_code;
    std::string m_buffer;
};

void CallbackTask::doResponse()
{
    m_cb->onRequestCompleted(m_code, m_buffer);
    delete this;
}

void CallbackThread::doResponse(base::RefPtr<RequestCallback> cb, int code, const std::string& buffer)
{
    CallbackTask* t = new CallbackTask(cb, code, buffer);
    base::RefPtr<base::TaskBase> task = new base::Task<CallbackTask>(t, &CallbackTask::doResponse);
    base::RunLoop* rl = runLoop();
    rl->postTask(task.get());
}

}
