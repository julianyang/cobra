/* 
 * Moudule: core
 * Author: cloudyang@2015/04/17
 */

#include "core/ClientNetContext.h"

#include "core/ClientNetworkThread.h"
#include "core/CallbackThread.h"
#include "core/Message.h"
#include "core/RouteManager.h"


namespace CobraCore {

ClientNetContext::ClientNetContext()
{

}

ClientNetContext::~ClientNetContext()
{
    stop();
}

void ClientNetContext::init(const std::string& config)
{
    RouteManager::getInstance()->initialize(config);

    int threadNum = 1;

    m_callbackThreads.resize(threadNum);
    for (int i=0; i<threadNum; i++)
        m_callbackThreads[i] = new CallbackThread;

    m_netThreads.resize(threadNum);
    for (int i=0; i<threadNum; i++)
        m_netThreads[i] = new ClientNetworkThread;
}

void ClientNetContext::start()
{
    for (size_t i=0; i<m_callbackThreads.size(); i++)
        m_callbackThreads[i]->start();

    for (size_t i=0; i<m_netThreads.size(); i++)
        m_netThreads[i]->start();
}

void ClientNetContext::stop()
{
    for (size_t i=0; i<m_netThreads.size(); i++) {
        m_netThreads[i]->stop();
        delete m_netThreads[i];
    }

    for (size_t i=0; i<m_callbackThreads.size(); i++) {
        m_callbackThreads[i]->stop();
        delete m_callbackThreads[i];
    }

    m_callbackThreads.clear();
    m_netThreads.clear();
}

void ClientNetContext::onConnectionCreate(ClientConnection* conn)
{
    m_netThreads[conn->getFd()%m_netThreads.size()]->addConnection(conn);
}

void ClientNetContext::onConnectionSend(SendMessage* m)
{
    m_netThreads[m->getConnection()->getFd()%m_netThreads.size()]->sendMessage(m);
}

void ClientNetContext::onResponse(ClientConnection* conn, int ret, const std::string& buffer)
{
    int index = conn->getFd() % m_netThreads.size();
    SendMessage* m = m_netThreads[index]->getSendMessage(conn->getUid());
    if (!m) // maybe timeout.
        return;

    if (m->getType() == SendMessage::SYNC) {
        m->setResult(ret);
        m->setResponseBuffer(buffer);
        m->notify();
    } else if (m->getType() == SendMessage::ONEWAY) {
        m_netThreads[index]->closeConnection(conn);
        delete m;
    } else {
        m_callbackThreads[m->getId()%m_callbackThreads.size()]->doResponse(m->getCallback(), ret, buffer);
        if (ret == 0)
            m_netThreads[index]->closeConnection(conn);
        delete m;
    }
}

void ClientNetContext::onConnectionClose(ClientConnection* conn)
{
    m_netThreads[conn->getFd()%m_netThreads.size()]->closeConnection(conn);
}

}
