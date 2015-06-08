/* 
 * Moudule: core
 * Author: cloudyang@2015/04/17
 */

#ifndef COBRA_CORE_SERVER_NETCONTEXT
#define COBRA_CORE_SERVER_NETCONTEXT

#include <string>
#include <vector>
#include <map>

#include "base/RefPtr.h"
#include "core/NetContext.h"
#include "core/PortManager.h"
#include "core/NetworkThread.h"

namespace CobraCore {

class Protocol;

class ServerNetContext : public NetContext, public PortManager::PortManagerClient, public NetworkThread::NetworkThreadClient {
public:
    ServerNetContext() { }
    ~ServerNetContext() { stop(); }

    // inherit from NetContext.
    virtual void init(const std::string& config);
    // call addHandle before start();
    virtual void addHandle(unsigned short port, RequestHandle* handle);
    
    virtual void start();
    virtual void stop();

    // inherit from PortManagerClient.
    virtual void onPortManagerStart(PortManager* p);
    virtual void onPortManagerStop(PortManager* p);
    virtual void closeConnection(int uid, int fd);
    virtual void sendResponse(int fd, SendPacket* p);

    // inherit from NetworkThreadClient. 
    virtual void addConnection(base::RefPtr<Connection>& conn);

private:
    std::vector<NetworkThread*> m_netThreads;
    std::map<unsigned short, base::RefPtr<PortManager> > m_portManagers;
    std::map<int, Protocol*> m_protocols;
};

}

#endif
