/* 
 * Moudule: core
 * Author: cloudyang@2015/04/17
 */

#ifndef COBRA_CORE_CONNECTION
#define COBRA_CORE_CONNECTION

#include <string>
#include "base/RefCountedThreadSafe.h"
#include "util/Socket.h"

namespace CobraCore {

class Connection : public base::RefCountedThreadSafe<Connection> {
public:
    class ConnectionClient {
    public:
        virtual void increaseConnection() = 0;
        virtual void decreaseConnection() = 0;
        virtual void onRecvCompleted(Connection* conn, std::string& recvBuffer) = 0;

    protected:
        ConnectionClient() {}
        virtual ~ConnectionClient() { }
    };

    explicit Connection(int fd, int listenFd, ConnectionClient* client, int timeout, const std::string& ip, const unsigned short port);

    virtual ~Connection();
    void close();

    int getFd() const { return m_socket.getFd(); }
    int getListenFd() const { return m_listenFd; }

    ConnectionClient* client() const { return m_client; }

    void setUid(int uid) { m_uid = uid; }
    int getUid() const { return m_uid; }

    int getTimeout() const { return m_timeout; }
    unsigned short getPort() const { return m_port; }
    std::string getIp() const { return m_ip; }

    int send();
    int send(const std::string& buffer);
    int recv();

protected:
    friend class base::RefCountedThreadSafe<Connection>;

    int m_uid;
    CobraUtil::Socket m_socket;
    int m_listenFd;
    int m_timeout;

    ConnectionClient* m_client;

    unsigned short m_port;
    std::string m_ip;

    size_t m_sendPos;
    std::string m_recvBuffer;
    std::string m_sendBuffer;
};

}

#endif
