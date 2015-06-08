#include <unistd.h>
#include "base/Singleton.h"
#include "base/Logger.h"
#include "base/RefPtr.h"
#include "util/UtilTools.h"
#include "core/ClientNetContext.h"
#include "core/RequestCallback.h"
#include "core/ServerProxy.h"

class MyLogger : public base::Singleton<MyLogger> {
public:
    MyLogger() : m_logger() { }
    const base::Logger& logger() const { return m_logger; }

private:
    base::Logger m_logger;
};

#define DEBUG_INFO MyLogger::getInstance()->logger().debug()

class MyCallback : public CobraCore::RequestCallback {
public:
    virtual void onRequestCompleted(int ret, std::string& response) 
    {
        DEBUG_INFO << "server response|ret:" << ret
            << "|content:" << response
            << std::endl;
    }
};


int main() 
{
    std::string config = "";
    CobraCore::ClientNetContext cnc;
    cnc.init(config);
    cnc.start();

    base::RefPtr<CobraCore::ServerProxy> sp = new CobraCore::ServerProxy(1, 0, &cnc);
    base::RefPtr<CobraCore::RequestCallback> cb = new MyCallback;

    std::string sendPrix = "request time:";

    int number = 10;
    for (int i=0; i<number; i++) {
        std::string request = sendPrix + CobraUtil::UtilTools::toString(i);
        sp->sendRequest(cb, request);
        DEBUG_INFO << "send message:" << request << std::endl;
        usleep(300000);
    }

    std::string oneway = "ONE way request";
    base::RefPtr<CobraCore::RequestCallback> nullCb = NULL;
    sp->sendRequest(nullCb, oneway);

    std::string sync = "SYNC request";
    std::string response;
    int ret = sp->sendRequest(sync, response);
    if (ret == 0)
        DEBUG_INFO << "recieve sync response:" << response;
    else 
        DEBUG_INFO << "sync request error:" << ret;

    usleep(2000000);

    cnc.stop();

    return 0;
}
