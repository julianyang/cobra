/* 
 * Moudule: core
 * Author: cloudyang@2015/05/21
 */

#ifndef COBRA_CORE_CLIENT_CONNECTION
#define COBRA_CORE_CLIENT_CONNECTION

#include <string>
#include "base/RefCountedThreadSafe.h"
#include "base/RefPtr.h"
#include "util/Socket.h"

namespace CobraCore {

class RequestCallback;

class ClientConnection : public base::RefCountedThreadSafe<ClientConnection> {
public:
    class ClientConnectionClient {
    public:
        virtual ~ClientConnectionClient() { }
        virtual void onRecvCompleted(ClientConnection* conn, std::string& recvBuffer) = 0;
        virtual void onException(ClientConnection* conn, int error) = 0;

    protected:
        ClientConnectionClient() {}
    };

    explicit ClientConnection(const std::string& ip, const unsigned short port, ClientConnectionClient* client);

    virtual ~ClientConnection();

    void close();

    bool isActive() const;
    bool connect(bool force);

    int getFd() const { return m_socket.getFd(); }

    ClientConnectionClient* client() const { return m_client; }

    unsigned short getPort() const { return m_port; }
    std::string getIp() const { return m_ip; }

    int send();
    int send(const std::string& buffer);
    int recv();

    void setUid(int uid) { m_uid = uid; }
    int getUid() const { return m_uid; }

    static void doException(base::RefPtr<ClientConnection> conn, int error);

protected:
    friend class base::RefCountedThreadSafe<ClientConnection>;

    void onException(int error);

    bool connectHost();
    bool resolveHost(const char* name, struct sockaddr_in* paddr); 

    CobraUtil::Socket m_socket;

    ClientConnectionClient* m_client;

    unsigned short m_port;
    std::string m_ip;

    int m_uid;

    size_t m_sendPos;
    std::string m_recvBuffer;
    std::string m_sendBuffer;
};

}

#endif
