/* 
 * Moudule: core
 * Author: cloudyang@2015/05/20
 */

#include "core/ServerProxy.h"

#include <assert.h>
#include "core/Protocol.h"
#include "core/Message.h"
#include "core/RequestCallback.h"
#include "core/RouteManager.h"
#include "core/ClientConnection.h"
#include "core/CoreLogger.h"

namespace CobraCore {

ServerProxy::ServerProxy(int module, int set, ServerProxyClient* client) 
    : m_module(module)
    , m_set(set)
    , m_timeout(30000)
    , m_protocol(createProtocol(0)) // use default protocol.
    , m_client(client)
{
    assert(client);

    CORE_DEBUG << "Create ServerProxy|module:" << m_module 
        << "|set:" << m_set
        << "|timeout:" << m_timeout
        << std::endl;
}

ServerProxy::~ServerProxy()
{
    delete m_protocol;

    CORE_DEBUG << "Destroy ServerProxy|module:" << m_module 
        << "|set:" << m_set
        << "|timeout:" << m_timeout
        << std::endl;
}

int ServerProxy::sendRequest(base::RefPtr<RequestCallback> cb, std::string& request)
{
    base::RefPtr<ClientConnection> conn = getConnection();
    if (!conn || !conn->connect(true))
        return -1;

    m_client->onConnectionCreate(conn.get());
    
    m_protocol->protocolEncode(request);

    // create SendMessage.
    SendMessage* m = new SendMessage(SendMessage::ASYNC, m_timeout, cb, conn);
    m->setRequestBuffer(request);

    m_client->onConnectionSend(m);

    return 0;
}

int ServerProxy::sendRequest(std::string& request, std::string& response)
{
    base::RefPtr<ClientConnection> conn = getConnection();
    if (!conn || !conn->connect(true))
        return -1;

    
    m_client->onConnectionCreate(conn.get());

    m_protocol->protocolEncode(request);
    
    // create SendMessage.
    base::RefPtr<RequestCallback> cb = NULL;
    SendMessage* m = new SendMessage(SendMessage::SYNC, m_timeout, cb, conn);
    m->setRequestBuffer(request);

    m_client->onConnectionSend(m);
    
    m->wait();

    m_client->onConnectionClose(conn.get());
    response = m->getResponseBuffer();
    return m->getResult();
}

base::RefPtr<ClientConnection> ServerProxy::getConnection() 
{
    RouteManager::RouteInfo info;
    info.m_module = m_module;
    info.m_set = m_set;

    if (!RouteManager::getInstance()->getRouteInfo(info)) {
        CORE_ERROR << "getRouteInfo error|module:" << m_module << "|set:" << m_set << std::endl;
        return NULL;
    }

    return new ClientConnection(info.m_hostIp, info.m_hostPort, this); 
}

void ServerProxy::onRecvCompleted(ClientConnection* conn, std::string& buffer)
{
    while (true) {
        std::string out;
        int ret = m_protocol->protocolParse(buffer, out);
        if (ret == Protocol::E_PROTOCOL_PARSE_LESS)
            break;
        else if (ret == Protocol::E_PROTOCOL_PARSE_OK) {
            m_client->onResponse(conn, 0, out);
            if (buffer.empty())
                break;
        } else {
            // in error case.
            CORE_ERROR << "recieve bad packet|fd:" << conn->getFd() << std::endl;
            return;
        }
    }
}

void ServerProxy::onException(ClientConnection* conn, int error)
{
    std::string buffer = "";
    m_client->onResponse(conn, error, buffer);
}

}
