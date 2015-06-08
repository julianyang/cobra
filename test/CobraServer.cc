#include <unistd.h>
#include "base/Singleton.h"
#include "base/Logger.h"
#include "base/RefPtr.h"
#include "base/RunLoop.h"
#include "core/ServerNetContext.h"
#include "core/RequestHandle.h"
#include "core/RequestContext.h"

class MyLogger : public base::Singleton<MyLogger> {
public:
    MyLogger() : m_logger() { }
    const base::Logger& logger() const { return m_logger; }

private:
    base::Logger m_logger;
};

#define DEBUG_INFO MyLogger::getInstance()->logger().debug()


class MyRequestHandle : public CobraCore::RequestHandle {
public:
    virtual void onRequest(base::RefPtr<CobraCore::RequestContext>& context)
    {
        DEBUG_INFO << "recieve request|id:" << context->getId()
            << "|ip:" << context->getIp()
            << "|port:" << context->getPort()
            << "|RecvTime:" << context->getRecvTime()
            << "|content:" << context->getRequestBuffer()
            << std::endl;

        std::string response = "Lazy server response.";
        context->sendResponse(response);
    }
};

int main() 
{
    std::string config = "";
    CobraCore::ServerNetContext snc;
    snc.init(config);
    CobraCore::RequestHandle* handle = new MyRequestHandle();
    snc.addHandle(10085, handle);
    snc.start();
    
    base::RunLoop runLoop;
    runLoop.run();

    snc.stop();

    return 0;
}
