/* 
 * Moudule: core
 * Author: cloudyang@2015/05/16
 */

#ifndef COBRA_CORE_REQUESTHANDLE_THREAD
#define COBRA_CORE_REQUESTHANDLE_THREAD

#include <map>
#include "core/OneWorkThreadProxy.h"


namespace CobraCore {

class RecvPacket;
class SendPacket;
class RequestHandle;

class RequestHandleThread : public OneWorkThreadProxy {
public:
    class RequestHandleThreadClient {
    public:
        virtual ~RequestHandleThreadClient() {}
        virtual bool receivePacket(RecvPacket*& p, int timeout) = 0;
        virtual void closeConnection(int uid, int fd) = 0;
        virtual void sendResponse(int fd, SendPacket* p) = 0;

    protected:
        RequestHandleThreadClient() {}
    };

    RequestHandleThread(RequestHandleThreadClient* clent);
    ~RequestHandleThread();

    void closeConnection(int uid, int fd) { m_client->closeConnection(uid, fd); }
    void sendResponse(int fd, SendPacket* p) { m_client->sendResponse(fd, p); }

    void addRequestHandle(RequestHandle* handle);

    virtual void doWork();

private:
    RequestHandleThreadClient* m_client;
    RequestHandle* m_handle;
};

}

#endif
