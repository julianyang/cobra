/* 
 * Moudule: core
 * Author: cloudyang@2015/05/20
 */

#ifndef COBRA_CORE_SERVERPROXY
#define COBRA_CORE_SERVERPROXY

#include <string>
#include <vector>
#include "base/RefCountedThreadSafe.h"
#include "base/RefPtr.h"
#include "core/ClientConnection.h"

namespace CobraCore {

class RequestCallback;
class SendMessage;
class Protocol;

class ServerProxy : public base::RefCountedThreadSafe<ServerProxy> 
    , public ClientConnection::ClientConnectionClient {
public:

    class ServerProxyClient {
    public:
        virtual ~ServerProxyClient() {}
        virtual void onConnectionCreate(ClientConnection* conn) = 0;
        virtual void onConnectionSend(SendMessage* m) = 0;
        virtual void onResponse(ClientConnection* conn, int ret, const std::string& buffer) = 0;
        virtual void onConnectionClose(ClientConnection* conn) = 0;

    protected:
        ServerProxyClient() {}
    };

    ServerProxy(int module, int set, ServerProxyClient* client);
    virtual ~ServerProxy();

    int getTimeout() const { return m_timeout; } 
    void setTimouet(int timeout) { m_timeout = timeout; }

    // async
    int sendRequest(base::RefPtr<RequestCallback> cb, std::string& request);
    // sync
    int sendRequest(std::string& request, std::string& response);

    // inherit from ClientConnectionClient.
    virtual void onRecvCompleted(ClientConnection* conn, std::string& buffer);
    virtual void onException(ClientConnection* conn, int error);

protected:

    friend class base::RefCountedThreadSafe<ServerProxy>;
    base::RefPtr<ClientConnection> getConnection();

private:
    int m_module;
    int m_set;
    int m_timeout;

    Protocol* m_protocol;
    ServerProxyClient* m_client;
};

}

#endif
