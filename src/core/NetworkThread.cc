/* 
 * Moudule: core
 * Author: cloudyang@2015/04/17
 */

#include "core/NetworkThread.h"

#include <assert.h>
#include <errno.h>
#include <arpa/inet.h>
#include "util/Socket.h"
#include "util/EventObserver.h"
#include "core/Packet.h"
#include "core/PortManager.h"
#include "core/CoreLogger.h"

namespace CobraCore {

enum NetworkEvent {
    E_EVENT_NET = 0, // default is net event.
    E_EVENT_LISTEN,
    E_EVENT_CLOSE,
    E_EVENT_NOTIFY
};

NetworkThread::NetworkThread(int index, NetworkThreadClient* client) 
    : OneWorkThreadProxy("netthread", 50)
    , m_client(client)
    , m_shutdown(new CobraUtil::Socket)
    , m_notify(new CobraUtil::Socket)
    , m_observer(new CobraUtil::EventObserver)
    , m_connectionList(new ConnectionList(index, this))
{
    assert(m_client);

    m_shutdown->create();
    m_notify->create();
    m_observer->create();

    CobraUtil::Event e;
    e.m_fd = m_shutdown->getFd();
    e.m_data = static_cast<long long>(E_EVENT_CLOSE) << 32;
    e.m_events = CobraUtil::Event::FD_EVENT_IN;
    
    m_observer->addEvent(e);

    e.m_fd = m_notify->getFd();
    e.m_data = static_cast<long long>(E_EVENT_NOTIFY) << 32;
    e.m_events = CobraUtil::Event::FD_EVENT_IN;

    m_observer->addEvent(e);

    CORE_DEBUG << "Create network thread|index:" << index << std::endl; 
}

NetworkThread::~NetworkThread()
{
    close();
    stop();

    delete m_connectionList;
    delete m_observer;
    delete m_notify;
    delete m_shutdown;

    CORE_DEBUG << "Destroy network thread" << std::endl;
}

void NetworkThread::close() 
{
    CobraUtil::Event e;
    e.m_fd = m_shutdown->getFd();
    e.m_data = static_cast<long long>(E_EVENT_CLOSE) << 32;
    e.m_events = CobraUtil::Event::FD_EVENT_IN;
    m_observer->modifyEvent(e);
}

void NetworkThread::notify() 
{
    CobraUtil::Event e;
    e.m_fd = m_notify->getFd();
    e.m_data = static_cast<long long>(E_EVENT_NOTIFY) << 32;
    e.m_events = CobraUtil::Event::FD_EVENT_IN;
    m_observer->modifyEvent(e);
}

void NetworkThread::addListener(base::RefPtr<PortManager>& listener)
{
    std::map<int, base::RefPtr<PortManager> >::iterator it = m_listeners.find(listener->getFd());
    if (it != m_listeners.end())
        return;

    m_listeners[listener->getFd()] = listener;
    
    CobraUtil::Event e;
    e.m_fd = listener->getFd();
    e.m_data = (static_cast<long long>(E_EVENT_LISTEN) << 32) | e.m_fd; 
    e.m_events = CobraUtil::Event::FD_EVENT_IN;
    m_observer->addEvent(e);

    CORE_DEBUG << "Add listener|port:" << listener->getPort() << std::endl; 
}

void NetworkThread::removeListener(base::RefPtr<PortManager>& listener)
{
    std::map<int, base::RefPtr<PortManager> >::iterator it = m_listeners.find(listener->getFd());
    if (it == m_listeners.end())
        return;

    m_listeners.erase(it);
    
    CobraUtil::Event e;
    e.m_fd = listener->getFd();
    e.m_data = (static_cast<long long>(E_EVENT_LISTEN) << 32) | e.m_fd; 
    e.m_events = CobraUtil::Event::FD_EVENT_IN;
    m_observer->removeEvent(e);
    
    CORE_DEBUG << "Remove listener|port:" << listener->getPort() << std::endl; 
}

void NetworkThread::closeConnection(int uid) 
{
    SendPacket* send = new SendPacket;
    send->m_id = uid;
    send->m_cmd = 'C';

    m_sendQueue.push_back(send);
    notify();
}

void NetworkThread::sendResponse(SendPacket* p)
{
    m_sendQueue.push_back(p);
    notify();
}

void NetworkThread::addConnection(base::RefPtr<Connection>& conn) 
{
    int uid = m_connectionList->add(conn, conn->getTimeout());
    conn->setUid(uid);
    conn->client()->increaseConnection();

    CobraUtil::Event e;
    e.m_fd = conn->getFd();
    e.m_data = conn->getUid();
    e.m_events = CobraUtil::Event::FD_EVENT_IN | CobraUtil::Event::FD_EVENT_OUT;
    m_observer->addEvent(e);

    CORE_DEBUG << "Add connection|fd:" << e.m_fd << std::endl; 
}

void NetworkThread::doWork() 
{
    // 1. check connection list timeout
    m_connectionList->checkTimeout();

    // 2. watch event.
    int size = m_observer->waitEvent(2000);
    CobraUtil::Event e;
    for (int index=0; index<size; index++) {
        m_observer->getEvent(index, e);
        // get network thread event type. 
        int eventType = e.m_data >> 32;
        switch (eventType) {
        case E_EVENT_LISTEN:
            handleAccept(e);
            break;
        case E_EVENT_CLOSE:
            // weakup epoll immediately.
            return;
        case E_EVENT_NOTIFY:
            handleNotify(e);
            break;
        case E_EVENT_NET:
            handleNet(e);
            break;
        default:
            // to do nothing.
            break;
        }
    }
}

void NetworkThread::timeout(base::RefPtr<Connection>& conn)
{
    if (!conn)
        return;

    if (conn->getListenFd() != -1) {
        // CompleteMe: send a close packet to app?
        conn->client()->decreaseConnection();
        
        // 1. remove it from epoll.
        CobraUtil::Event e;
        e.m_fd = conn->getFd();
        e.m_data = conn->getUid();
        e.m_events = CobraUtil::Event::FD_EVENT_IN | CobraUtil::Event::FD_EVENT_OUT;
        m_observer->removeEvent(e);

        // 2. close socket.
        conn->close();

        CORE_ERROR << "Connection timeout|fd:" << e.m_fd << std::endl; 
    }
}

void NetworkThread::closeConnection(base::RefPtr<Connection>& conn) 
{
    if (conn->getListenFd() != -1) {
        // CompleteMe: send a close packet to app?
        conn->client()->decreaseConnection();
        
        // 1. remove it from epoll.
        CobraUtil::Event e;
        e.m_fd = conn->getFd();
        e.m_data = conn->getUid();
        e.m_events = CobraUtil::Event::FD_EVENT_IN | CobraUtil::Event::FD_EVENT_OUT;
        m_observer->removeEvent(e);

        // 2. close socket.
        conn->close();

        // 3. remvoe from connection list.
        m_connectionList->del(conn->getUid());

        CORE_ERROR << "Close connection|fd:" << e.m_fd << std::endl; 
    }
}

void NetworkThread::handleAccept(const CobraUtil::Event& e) 
{
    if (!(e.m_events & CobraUtil::Event::FD_EVENT_IN))
        return;

    int fd = static_cast<int>(e.m_data);
    std::map<int, base::RefPtr<PortManager> >::iterator it = m_listeners.find(fd);
    if (it == m_listeners.end())
        return;

    do {
        CobraUtil::Socket l(fd);
        struct sockaddr_in clientAddr;
        int length = sizeof(clientAddr);
        CobraUtil::Socket client = l.accept(reinterpret_cast<struct sockaddr*>(&clientAddr), &length); 
        if (client.getFd() == -1) {
            if (errno == EAGAIN)
                break;
            else
                continue;
        }   

        char* clientIp = inet_ntoa(clientAddr.sin_addr);
        unsigned short port = ntohs(clientAddr.sin_port);

        CORE_DEBUG << "Recieve request|ip:" << clientIp << "|port:" << port << std::endl;

        // 1. check client by PortManager;
        std::string ip = clientIp;
        if (!it->second->canAccept(ip) || it->second->isOverConnections()) {
            client.close();
            CORE_ERROR << "Refuse request|ip:" << ip << "|port:" << port << std::endl;
            continue;
        } 

        client.setBlock(false);
        client.setKeepAlive();
        client.setTcpNoDelay();
        client.setCloseWait();

        int timeout = it->second->getQueueTimeout();
        base::RefPtr<Connection> conn = new Connection(client.getFd(), fd/*listen fd*/, it->second.get(), timeout, ip, port);
        // add to net thread.
        m_client->addConnection(conn);
    } while (true);
    return;
}

void NetworkThread::handleNotify(const CobraUtil::Event& e)
{
    // In epoll et mode, we need trying our best to send data, untill return EAGAIN.
    // since we can catch OUT event only in buffer status change case.
    std::deque<SendPacket*> sendQueue;
    m_sendQueue.swap(sendQueue);

    std::deque<SendPacket*>::iterator it = sendQueue.begin();
    while (it != sendQueue.end()) {
        switch ((*it)->m_cmd) {
        case 'c': {// close current connection.
            base::RefPtr<Connection> conn = m_connectionList->get((*it)->m_id);
                closeConnection(conn);
            break;
        }
        case 's': {
            base::RefPtr<Connection> conn = m_connectionList->get((*it)->m_id);
            if (conn) {
                int ret = conn->send((*it)->m_buffer);
                if (ret < 0)
                    closeConnection(conn);
            }
            break;
        }
        default:
            // not touched.
            break;
        }
        delete *it;
        it++;
    }
}

void NetworkThread::handleNet(const CobraUtil::Event& e) 
{
    base::RefPtr<Connection> conn = m_connectionList->get(e.m_data);
    if (!conn)
        return;

    if (e.m_events & CobraUtil::Event::FD_EVENT_HUP || e.m_events & CobraUtil::Event::FD_EVENT_ERR) {
        closeConnection(conn);
        return;
    }   

    if (e.m_events & CobraUtil::Event::FD_EVENT_IN) { // recv packet for current connection
        int ret = conn->recv();
        if (ret < 0) {
            closeConnection(conn);
            return;
        }
    } 

    if (e.m_events & CobraUtil::Event::FD_EVENT_OUT) { // send packet for current connection
        int ret = conn->send();
        if (ret < 0) {
            closeConnection(conn);
            return;
        }
    } 

    m_connectionList->refresh(e.m_data, conn->getTimeout());
}

}
