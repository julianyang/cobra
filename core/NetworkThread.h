/* 
 * Moudule: core
 * Author: cloudyang@2015/04/17
 */

#ifndef COBRA_CORE_NETWORK_THREAD
#define COBRA_CORE_NETWORK_THREAD

#include <map>
#include "base/RefPtr.h"
#include "util/ThreadQueue.h"
#include "core/OneWorkThreadProxy.h"
#include "core/ConnectionList.h"

namespace CobraUtil {
    class Socket;
    struct Event;
    class EventObserver;
}

namespace CobraCore {

class SendPacket;
class PortManager;
class Connection;


class NetworkThread : public OneWorkThreadProxy, public ConnectionList::ConnectionListClient {
public:
    class NetworkThreadClient {
    public:
        virtual void addConnection(base::RefPtr<Connection>& conn) = 0;

    protected:
        NetworkThreadClient() {}
        virtual ~NetworkThreadClient() {}
    };

    NetworkThread(int index, NetworkThreadClient* client);
    ~NetworkThread();

    // call on net work thread.
    virtual void doWork();

    virtual void timeout(base::RefPtr<Connection>& conn);

    void close();
    void notify();
    void addListener(base::RefPtr<PortManager>& listener);
    void removeListener(base::RefPtr<PortManager>& listener);
    void addConnection(base::RefPtr<Connection>& conn);
    void closeConnection(int uid);
    void sendResponse(SendPacket* p);

    ConnectionList* getConnectionList() { return m_connectionList; }

private:
    void closeConnection(base::RefPtr<Connection>& conn);

    void handleAccept(const CobraUtil::Event& event);
    void handleNotify(const CobraUtil::Event& event);
    void handleNet(const CobraUtil::Event& event);

    NetworkThreadClient* m_client;
    CobraUtil::Socket* m_shutdown;
    CobraUtil::Socket* m_notify;
    CobraUtil::EventObserver* m_observer;

    CobraUtil::ThreadQueue<SendPacket*> m_sendQueue;
    
    std::map<int, base::RefPtr<PortManager> > m_listeners;
    ConnectionList* m_connectionList;
};

}

#endif
