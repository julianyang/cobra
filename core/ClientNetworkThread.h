/* 
 * Moudule: core
 * Author: cloudyang@2015/05/20
 */

#ifndef COBRA_CORE_CLIENT_NETWORK_THREAD
#define COBRA_CORE_CLIENT_NETWORK_THREAD

#include <map>
#include "util/ThreadQueue.h"
#include "util/TimeoutQueueUD.h"
#include "core/OneWorkThreadProxy.h"

namespace CobraUtil {
    class Socket;
    struct Event;
    class EventObserver;
}

namespace CobraCore {

class SendMessage;
class ClientConnection;

class ClientNetworkThread : public OneWorkThreadProxy {
public:

    ClientNetworkThread();
    ~ClientNetworkThread();

    // call on net work thread.
    virtual void doWork();

    void close();
    void notify();

    void addConnection(ClientConnection* conn);
    void closeConnection(ClientConnection* conn);
    void sendMessage(SendMessage* m);

    SendMessage* getSendMessage(int uid);


    static void timeoutHandler(SendMessage*& m, void* p);

private:

    void handleException(SendMessage* m, int type);
    void handleNotify();
    void handle(const CobraUtil::Event& event);
    void handleSendMessage(SendMessage* m);
    void handleRecv(ClientConnection* conn);
    void handleSend(ClientConnection* conn);

    CobraUtil::Socket* m_shutdown;
    CobraUtil::Socket* m_notify;
    CobraUtil::EventObserver* m_observer;

    // SendMessage timeout queue.
    CobraUtil::ThreadQueue<SendMessage*> m_messageQueue;
    CobraUtil::TimeoutQueueUD<SendMessage*> m_timeoutQueue;
};

}

#endif
