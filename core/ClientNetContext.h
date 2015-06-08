/* 
 * Moudule: core
 * Author: cloudyang@2015/04/17
 */

#ifndef COBRA_CORE_CLIENT_NETCONTEXT
#define COBRA_CORE_CLIENT_NETCONTEXT

#include <string>
#include <vector>
#include "core/NetContext.h"
#include "core/ServerProxy.h"

namespace CobraCore {

class ClientNetworkThread;
class CallbackThread;
class SendMessage;

class ClientNetContext : public NetContext, public ServerProxy::ServerProxyClient {
public:
    ClientNetContext();
    ~ClientNetContext();

    virtual void init(const std::string& config);
    virtual void start();
    virtual void stop();


    virtual void onConnectionCreate(ClientConnection* conn);
    virtual void onConnectionSend(SendMessage* m);
    virtual void onResponse(ClientConnection* conn, int ret, const std::string& buffer);
    virtual void onConnectionClose(ClientConnection* conn);

private:
    std::vector<ClientNetworkThread*> m_netThreads;
    std::vector<CallbackThread*> m_callbackThreads;
};

}

#endif
