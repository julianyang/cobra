/* 
 * Moudule: core
 * Author: cloudyang@2015/05/21
 */

#include "core/ClientConnection.h"

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "core/CoreLogger.h"

namespace CobraCore {

static const int sMaxBufferLength = 8192;

ClientConnection::ClientConnection(const std::string& ip, unsigned short port, ClientConnectionClient* client)
    : m_socket()
    , m_client(client)
    , m_port(port)
    , m_ip(ip)
    , m_uid(0)
    , m_sendPos(0)
{
    assert(client);

    CORE_DEBUG << "Create client connection|ip:" << m_ip << "|port:" << m_port << std::endl;
}

ClientConnection::~ClientConnection() 
{
    m_socket.close();
    CORE_DEBUG << "Destroy client connection|ip:" << m_ip << "|port:" << m_port << std::endl;
}

void ClientConnection::close() 
{
    m_socket.close();
}

bool ClientConnection::isActive() const 
{
    int val = 0;
    socklen_t length = sizeof(val);
    return m_socket.getFd() != CobraUtil::INVALD_SOCKET
        && (m_socket.getSocketOpt(SOL_SOCKET, SO_ERROR, reinterpret_cast<void*>(&val), &length) > 0 && val == 0); 
}


bool ClientConnection::resolveHost(const char* name, struct sockaddr_in* paddr)
{
    struct in_addr ip;
    struct hostent *hostp = NULL;

    if (inet_aton(name, &ip) != 0)
        hostp = gethostbyaddr(reinterpret_cast<const char*>(&ip), sizeof(ip), AF_INET);
    else
        hostp = gethostbyname(name);

    if (!hostp) { 
        CORE_ERROR << "Can not resolve host|name:" << name << std::endl;
        return false;
    }

    bzero(reinterpret_cast<char*>(paddr), sizeof(struct sockaddr_in));
    paddr->sin_family = AF_INET;
    bcopy(reinterpret_cast<char*>(hostp->h_addr), reinterpret_cast<char*>(&(paddr->sin_addr.s_addr)), hostp->h_length);

    return true;
}

bool ClientConnection::connectHost() 
{
    struct sockaddr_in addr;
    if (!resolveHost(m_ip.c_str(), &addr))
        return false;  

    addr.sin_port = htons(m_port);

    try {
        m_socket.connect(reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));

        m_socket.setBlock(false);
        m_socket.setCloseWait();
        m_socket.setKeepAlive();
        m_socket.setTcpNoDelay();
    } catch (CobraUtil::UtilException& e) {
        m_socket.close();
        CORE_ERROR << "Connect cause exception:" << e.what() << "|ip:" << m_ip << "|port:" << m_port << std::endl;
        return false;
    }

    return true;
}

bool ClientConnection::connect(bool force) 
{
    if (force) {
        m_socket.close();
        m_socket.create();
        return connectHost();
    }

    if (isActive())
        return true;

    m_socket.close();
    m_socket.create();
    return connectHost();
}

int ClientConnection::send() 
{
    if (m_sendBuffer.length() == 0) 
        return 0;

    return send("");
}

int ClientConnection::send(const std::string& buffer)
{
   m_sendBuffer += buffer;
   size_t sendLen = m_sendBuffer.length() - m_sendPos;
   const char* sendBuffer = m_sendBuffer.c_str() + m_sendPos;

   size_t currentPos = 0;
   while (currentPos < sendLen) {
       int sendBytes = ::write(m_socket.getFd(), sendBuffer + currentPos, sendLen - currentPos);
       if (sendBytes < 0) {
           if (errno == EAGAIN)
               break;
           else
               return -1;
       }

       currentPos += sendBytes;
       if (currentPos < sendLen)
           break; // wait for next OUT event.
   }

   if (currentPos > 0)
       m_sendPos += currentPos;

   if (m_sendBuffer.length() == m_sendPos) {
       m_sendBuffer.clear();
       m_sendPos = 0;
   }

   return currentPos;
}

int ClientConnection::recv()
{
    char buffer[sMaxBufferLength] = { 0 };

    while (true) {
        int recvBytes = ::read(m_socket.getFd(), buffer, sMaxBufferLength);
        if (recvBytes < 0) {
            if (errno == EAGAIN)
                return 0; // wait for next IN event.
            else
                return -1;
        } else if (recvBytes == 0) {
            return -1; // client closed this connection.
        } else {
            m_recvBuffer.append(buffer, recvBytes);
            if (recvBytes < sMaxBufferLength)
                break;
        }
    }

    // pase packet and insert it to thread queue.
    int ret = m_recvBuffer.length();
    m_client->onRecvCompleted(this, m_recvBuffer);
    return ret;
}


void ClientConnection::doException(base::RefPtr<ClientConnection> conn, int error)
{
   conn->onException(error); 
}

void ClientConnection::onException(int error) 
{
    m_client->onException(this, error);
}

}
