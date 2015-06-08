/* 
 * Moudule: core
 * Author: cloudyang@2015/04/17
 */

#include "core/ServerNetContext.h"

#include "core/Protocol.h"
#include "core/Connection.h"

namespace CobraCore {

void ServerNetContext::init(const std::string& config)
{
    // 1. parse config use json parser.
    unsigned short port = 10085;
    int protocol = 0;
    //int timeout = 60000;
    //int maxQueueSize = 10000;
    //int maxConnections = 1000;
    //int useReverseFilter = 0;
    std::vector<std::string> ips;
    
    size_t portInfoSize = 1;
    size_t networkThreadNum = 1;
    int handleNumber = 1;

    // 2. create network thread.
    m_netThreads.resize(networkThreadNum);
    for (size_t j=0; j<networkThreadNum; j++)
        m_netThreads[j] = new NetworkThread(static_cast<int>(j), this);

    // 3. create PortManager.
    for (size_t i=0; i<portInfoSize; i++) {
        std::map<int, Protocol*>::iterator it = m_protocols.find(protocol);
        if (it == m_protocols.end()) {
            m_protocols.insert(std::make_pair(protocol, createProtocol(protocol)));
            it = m_protocols.find(protocol);
        }
        it->second->initialize(config);
        m_portManagers[port] = new PortManager(port, handleNumber, it->second, this);
    }
}

void ServerNetContext::addHandle(unsigned short port, RequestHandle* handle) 
{
    std::map<unsigned short, base::RefPtr<PortManager> >::iterator it = m_portManagers.find(port);
    if (it != m_portManagers.end())
        (it->second)->addHandle(handle);
}

void ServerNetContext::start()
{
    for (size_t i=0; i<m_netThreads.size(); i++)
        m_netThreads[i]->start();

    std::map<unsigned short, base::RefPtr<PortManager> >::iterator it = m_portManagers.begin();
    while (it != m_portManagers.end()) {
        (it->second)->initialize();
        it++;
    }
}

void ServerNetContext::stop()
{
    // delete portmanager before stop net thread.
    m_portManagers.clear();

    for (size_t i=0; i<m_netThreads.size(); i++)
        m_netThreads[i]->stop();

    std::map<int, Protocol*>::iterator it = m_protocols.begin();
    while (it != m_protocols.end()) {
        delete it->second;
        it++;
    }

    m_netThreads.clear();
    m_protocols.clear();
}

void ServerNetContext::onPortManagerStart(PortManager* p)
{
    int netThread = p->getFd() % m_netThreads.size();
    base::RefPtr<PortManager> listener(p);
    m_netThreads[netThread]->addListener(listener);
}

void ServerNetContext::onPortManagerStop(PortManager* p)
{
    int netThread = p->getFd() % m_netThreads.size();
    base::RefPtr<PortManager> listener(p);
    m_netThreads[netThread]->removeListener(listener);
}

void ServerNetContext::closeConnection(int uid, int fd) 
{
    m_netThreads[fd % m_netThreads.size()]->closeConnection(uid);
}

void ServerNetContext::sendResponse(int fd, SendPacket* p)
{
    m_netThreads[fd % m_netThreads.size()]->sendResponse(p);
} 

void ServerNetContext::addConnection(base::RefPtr<Connection>& conn)
{
    m_netThreads[conn->getFd() % m_netThreads.size()]->addConnection(conn);
}

}
