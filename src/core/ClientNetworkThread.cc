/* 
 * Moudule: core
 * Author: cloudyang@2015/05/20
 */

#include "core/ClientNetworkThread.h"

#include "util/EventObserver.h"
#include "util/Socket.h"
#include "core/Message.h"
#include "core/ClientConnection.h"
#include "core/CoreLogger.h"


namespace CobraCore {

ClientNetworkThread::ClientNetworkThread() 
    : OneWorkThreadProxy("clientNetThread", 50)
    , m_shutdown(new CobraUtil::Socket)
    , m_notify(new CobraUtil::Socket)
    , m_observer(new CobraUtil::EventObserver)
{
    m_shutdown->create();
    m_notify->create();
    m_observer->create();

    CobraUtil::Event e;
    e.m_fd = m_shutdown->getFd();
    e.m_data = e.m_fd;
    e.m_events = CobraUtil::Event::FD_EVENT_IN;

    m_observer->addEvent(e);

    e.m_fd = m_notify->getFd();
    e.m_data = e.m_fd;
    m_observer->addEvent(e);

    CORE_DEBUG << "Create client network thread." << std::endl;
}

ClientNetworkThread::~ClientNetworkThread()
{
    close();
    stop();

    delete m_observer;
    delete m_notify;
    delete m_shutdown;

    CORE_DEBUG << "Destroy client network thread." << std::endl;
}

void ClientNetworkThread::close() 
{
    CobraUtil::Event e;
    e.m_fd = m_shutdown->getFd();
    e.m_data = e.m_fd;
    e.m_events = CobraUtil::Event::FD_EVENT_IN;
    m_observer->modifyEvent(e);
}

void ClientNetworkThread::notify()
{
    CobraUtil::Event e;
    e.m_fd = m_notify->getFd();
    e.m_data = e.m_fd;
    e.m_events = CobraUtil::Event::FD_EVENT_IN;
    m_observer->modifyEvent(e);
}

void ClientNetworkThread::addConnection(ClientConnection* conn)
{
    CobraUtil::Event e; 
    e.m_fd = conn->getFd();
    e.m_data = reinterpret_cast<long long>(conn);
    e.m_events =  CobraUtil::Event::FD_EVENT_IN | CobraUtil::Event::FD_EVENT_OUT;
    m_observer->addEvent(e);
   
    CORE_DEBUG << "Add client Connection|fd:" << e.m_fd << std::endl;
}

void ClientNetworkThread::closeConnection(ClientConnection* conn)
{
    CobraUtil::Event e; 
    e.m_fd = conn->getFd();
    e.m_data = reinterpret_cast<long long>(conn);
    e.m_events =  CobraUtil::Event::FD_EVENT_IN | CobraUtil::Event::FD_EVENT_OUT;
    m_observer->removeEvent(e);

    CORE_DEBUG << "Remove client Connection|fd:" << e.m_fd << std::endl;
}

SendMessage* ClientNetworkThread::getSendMessage(int uid)
{
    return m_timeoutQueue.get(uid);
} 

void ClientNetworkThread::sendMessage(SendMessage* m)
{
    if (!m)
        return;

    m_messageQueue.push_back(m);
    notify();
}

void ClientNetworkThread::timeoutHandler(SendMessage*& m, void* p)
{
    reinterpret_cast<ClientNetworkThread*>(p)->handleException(m, SendMessage::E_TIMEOUT);
}

void ClientNetworkThread::handleNotify() 
{
    SendMessage* m = NULL;
    while (m_messageQueue.pop_front(m))
        handleSendMessage(m);
}

void ClientNetworkThread::handleException(SendMessage* m, int type)
{
    if (!m)
        return;

    m_timeoutQueue.erase(m->getId()); // try to remove it from timeout queue.
    m->onException(type);
    closeConnection(m->getConnection());

    CORE_ERROR << "Connection exception|fd:" << m->getConnection()->getFd()
        << "|code:" << type
        << std::endl;

    if (m->getType() != SendMessage::SYNC)
        delete m;
}

void ClientNetworkThread::handle(const CobraUtil::Event& event)
{
    ClientConnection* conn = reinterpret_cast<ClientConnection*>(event.m_data);
    if (!conn)
        return;

    if (event.m_events & CobraUtil::Event::FD_EVENT_HUP || event.m_events & CobraUtil::Event::FD_EVENT_ERR) {
        SendMessage* m = m_timeoutQueue.get(conn->getUid());
        handleException(m, SendMessage::E_SOCKET_ERROR);
        return;
    }

    if (event.m_events & CobraUtil::Event::FD_EVENT_IN) 
        handleRecv(reinterpret_cast<ClientConnection*>(event.m_data));

    if (event.m_events & CobraUtil::Event::FD_EVENT_OUT)
        handleSend(reinterpret_cast<ClientConnection*>(event.m_data));
}

void ClientNetworkThread::handleRecv(ClientConnection* conn)
{
    if (conn->recv() < 0) {
        SendMessage* m = m_timeoutQueue.get(conn->getUid());
        handleException(m, SendMessage::E_RECV_ERROR);
    }
}

void ClientNetworkThread::handleSend(ClientConnection* conn)
{
    if (conn->send() < 0) {
        SendMessage* m = m_timeoutQueue.get(conn->getUid());
        handleException(m, SendMessage::E_SEND_ERROR);
    }
}

void ClientNetworkThread::handleSendMessage(SendMessage* m)
{
    m->setId(m_timeoutQueue.generateId());
    if (!m_timeoutQueue.push(m, m->getId(), m->getTimeout())) {
        handleException(m, SendMessage::E_QUEUE_FULL);
        return;
    }

    m->getConnection()->setUid(m->getId());

    if (m->getConnection()->send(m->getRequestBuffer()) < 0) {
        handleException(m, SendMessage::E_SEND_ERROR);
        return;
    }
}

void ClientNetworkThread::doWork() 
{
    m_timeoutQueue.timeout(timeoutHandler, reinterpret_cast<void*>(this));

    int size = m_observer->waitEvent(2000);
    CobraUtil::Event e;
    for (int index=0; index<size; index++) {
        m_observer->getEvent(index, e);
        if (e.m_data == m_shutdown->getFd())
            break;
        else if (e.m_data == m_notify->getFd())
            handleNotify();
        else
            handle(e);
    }
}

}
