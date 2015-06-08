/* 
 * Moudule: core
 * Author: cloudyang@2015/04/17
 */

#include "core/Connection.h"

#include <assert.h>
#include <errno.h>
#include "core/CoreLogger.h"

namespace CobraCore {

namespace {
    const int sMaxBufferLength = 8196;
}

Connection::Connection(int fd, int listenFd, ConnectionClient* client, int timeout, const std::string& ip, const unsigned short port)
    : m_uid(-1)
    , m_socket()
    , m_listenFd(listenFd)
    , m_timeout(timeout)
    , m_client(client)
    , m_port(port)
    , m_ip(ip)
    , m_sendPos(0)
{
    assert(client);
    assert(fd != -1);

    m_socket.init(fd, true);

    CORE_DEBUG << "Create connection|fd:" << fd << "|listenFd:" << m_listenFd << std::endl; 
}

Connection::~Connection() 
{
    CORE_DEBUG << "Destroy connection|listenFd:" << m_listenFd << std::endl; 
}

void Connection::close()
{
    m_socket.close();
}

int Connection::send() 
{
    if (m_sendBuffer.length() == 0) 
        return 0;

    return send("");
}

int Connection::send(const std::string& buffer)
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

int Connection::recv() 
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

}
