/* 
 * Moudule: core
 * Author: cloudyang@2015/04/17
 */


#include "core/PortManager.h"

#include <assert.h>
#include "base/Time.h"
#include "core/Packet.h"
#include "core/Protocol.h"
#include "core/CoreLogger.h"

namespace CobraCore {

PortManager::PortManager(unsigned short port, int handleNum, Protocol* p, PortManagerClient* client)
    : m_capacity(10000)
    , m_timeout(60000)
    , m_maxConnections(10000)
    , m_heartBeatInterval(60000)
    , m_port(port)
    , m_socket()
    , m_connectionLock()
    , m_connections(0)
    , m_protocol(p)
    , m_handle(NULL)
    , m_client(client)
    , m_requestList()
{
    assert(m_protocol);
    assert(m_client);

    if (handleNum <= 0)
        handleNum = 1;
    
    m_handles.resize(handleNum);
    for (int i=0; i<handleNum; i++)
        m_handles[i] = new RequestHandleThread(this);

    CORE_DEBUG << "Create PortManager|capacity:" << m_capacity
        << "|timeout:" << m_timeout
        << "|maxConnections:" << m_maxConnections
        << "|heartBeatInerval:" << m_heartBeatInterval
        << "|port:" << m_port
        << "|HandleNum:" << handleNum
        << std::endl;
}

PortManager::~PortManager() 
{
    for (size_t i=0; i<m_handles.size(); i++) {
        m_handles[i]->stop();
        delete m_handles[i];
    }

    m_handles.clear();

    m_client->onPortManagerStop(this);
    m_socket.close(); 

    CORE_DEBUG << "Destroy PortManager|capacity:" << m_capacity
        << "|timeout:" << m_timeout
        << "|maxConnections:" << m_maxConnections
        << "|heartBeatInerval:" << m_heartBeatInterval
        << "|port:" << m_port
        << std::endl;
}

bool PortManager::initialize()
{ 
    assert(m_handle);

    try {
        m_socket.close();

        m_socket.create();
        m_socket.bind(m_port);
        m_socket.listen(1024);
        m_socket.setBlock(false);
        m_socket.setCloseWait();
        m_socket.setKeepAlive();
        m_socket.setTcpNoDelay();

        m_client->onPortManagerStart(this);

        for (size_t i=0; i<m_handles.size(); i++) {
            m_handles[i]->addRequestHandle(m_handle);
            m_handles[i]->start();
        }
    } catch (CobraUtil::SocketException& e) {
        // ComplteMe: add error log here.
        CORE_ERROR << "PortManager init cause exception:" << e.what() << "|port:" << m_port << std::endl;
        return false;
    }

    return true;
}

int PortManager::getFd() const
{
    return m_socket.getFd();
}

bool PortManager::isOverConnections() const 
{
    base::AutoLock lock(const_cast<base::Lock&>(m_connectionLock));
    return m_connections >= m_maxConnections;
}

bool PortManager::isOverload() const 
{
    return m_capacity > 0 && m_requestList.size() > (m_capacity >> 1);
}

bool PortManager::canAccept(const std::string& ip) 
{
    return m_protocol->canAccept(ip);
}

void PortManager::addRecvPacket(std::deque<RecvPacket*>& ps, bool pushBack)
{
    if (pushBack)
        m_requestList.push_back(ps);
    else 
        m_requestList.push_front(ps);
}

bool PortManager::waitRecvPacket(RecvPacket* &p, int ms)
{
    return m_requestList.pop_front(p, ms);
}

void PortManager::increaseConnection()
{
    base::AutoLock lock(m_connectionLock);
    ++m_connections;
}

void PortManager::decreaseConnection()
{
    base::AutoLock lock(m_connectionLock);
    --m_connections;
}

void PortManager::onRecvCompleted(Connection* conn, std::string& buffer)
{
    std::deque<RecvPacket*> ps;
    while (true) {
        std::string out;
        int ret = m_protocol->protocolParse(buffer, out);
        if (ret == Protocol::E_PROTOCOL_PARSE_LESS)
            break;
        else if (ret == Protocol::E_PROTOCOL_PARSE_OK) {
            RecvPacket *p = new RecvPacket;
            p->m_id = conn->getUid();
            p->m_recvTime = base::Time::now().toInternalValue() / base::Time::kMicrosecondsPerMillisecond;
            //p.m_listenFd = conn->getListenFd();
            p->m_timeout = conn->getTimeout();
            p->m_fd = conn->getFd();
            p->m_port = conn->getPort();
            p->m_ip = conn->getIp();
            p->m_buffer = out;

            ps.push_back(p);
            
            if (buffer.empty())
                break;
        } else {
            // in error case.
            CORE_ERROR << "Recieve bad packet|buffer:" << buffer << std::endl;
            return;
        }
    }

    if (ps.size() > 0)
        addRecvPacket(ps);
}


void PortManager::sendResponse(int fd, SendPacket* p)
{
    m_protocol->protocolEncode(p->m_buffer);
    m_client->sendResponse(fd, p);
}

}
