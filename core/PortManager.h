/*
 * Moudule: core
 * Author: cloudyang@2015/04/17
 */

#ifndef COBRA_CORE_PORTMANAGER
#define COBRA_CORE_PORTMANAGER

#include <string>
#include "base/RefCountedThreadSafe.h"
#include "base/Lock.h"
#include "util/ThreadQueue.h"
#include "core/Connection.h"
#include "core/RequestHandleThread.h"

namespace CobraCore {

class Protocol;
class RecvPacket;
class RequestHandle;

class PortManager : public base::RefCountedThreadSafe<PortManager>
    , public Connection::ConnectionClient
    , public RequestHandleThread::RequestHandleThreadClient {
public:

    class PortManagerClient {
    public:
        virtual ~PortManagerClient() { }

        virtual void onPortManagerStart(PortManager* p) = 0;
        virtual void onPortManagerStop(PortManager* p) = 0;
        virtual void closeConnection(int uid, int fd) = 0;
        virtual void sendResponse(int fd, SendPacket* p) = 0;

    protected:
         PortManagerClient() { }
    };

    explicit PortManager(unsigned short port, int handleNum, Protocol* p, PortManagerClient* client);

    virtual ~PortManager();

    void addHandle(RequestHandle* handle) { m_handle = handle; }

    bool initialize();

    int getFd() const;

    int getPort() const { return m_port; }
    
    void setQueueCapacity(const size_t size = 10000) { m_capacity = size; }
    size_t getQueueCapacity() const { return m_capacity; }

    void setQueueTimeout(const int ms = 60000) { m_timeout = ms; }
    int getQueueTimeout() const { return m_timeout; }

    bool isOverConnections()  const;

    void setMaxConnections(const size_t size) { m_maxConnections = size; }
    size_t getMaxConnections() const { return m_maxConnections; }

    void setHeartBeatInterval(const int ms = 60000) { m_heartBeatInterval = ms; }
    int getHeartBeatInterval() const { return m_heartBeatInterval; } 

    bool isOverload() const;

    bool canAccept(const std::string& ip);

    void addRecvPacket(std::deque<RecvPacket*>& p, bool pushBack = true);
    bool waitRecvPacket(RecvPacket*& p, int ms);

    // inherit from ConnectionClient
    virtual void increaseConnection();
    virtual void decreaseConnection();
    virtual void onRecvCompleted(Connection* conn, std::string& buffer);

    // inherit from RequestHandleThreadClient.
    virtual bool receivePacket(RecvPacket*& p, int timeout) { return waitRecvPacket(p, timeout); }
    virtual void closeConnection(int uid, int fd) { m_client->closeConnection(uid, fd); }
    virtual void sendResponse(int fd, SendPacket* p);

protected:
    friend class base::RefCountedThreadSafe<PortManager>;

private:
    size_t m_capacity;
    int m_timeout;
    size_t m_maxConnections;
    int m_heartBeatInterval;
    bool m_overload;
    unsigned short m_port;
    CobraUtil::Socket m_socket;

    base::Lock m_connectionLock;

    size_t m_connections;

    Protocol* m_protocol;
    RequestHandle* m_handle;
    PortManagerClient* m_client;
    std::vector<RequestHandleThread*> m_handles;
    CobraUtil::ThreadQueue<RecvPacket*> m_requestList;
};

}

#endif
