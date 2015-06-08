/* 
 * Moudule: util
 * Author: cloudyang@2015/04/17
 */

#ifndef COBRA_UTIL_SOCKET
#define COBRA_UTIL_SOCKET

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include "util/UtilException.h"


namespace CobraUtil {

class SocketException : public UtilException {
public:
    SocketException(const std::string& message, int code = 0) : UtilException(message, code) { }
    ~SocketException() throw() { }
};

static const int INVALD_SOCKET = -1;

class Socket {
public:
    Socket() : m_owner(true), m_fd(INVALD_SOCKET), m_domain(AF_INET) { }
    Socket(int fd) : m_owner(false), m_fd(fd), m_domain(AF_INET) { }
    void init(int fd, bool owner = false, int domain = AF_INET);
    ~Socket() { if (m_owner) ::close(m_fd); }

    void setDomain(int domain) { m_domain = domain; }
    void setOwner(bool owner) { m_owner = owner; }

    void create(int domain = AF_INET, int type = SOCK_STREAM, int protocol = 0);
    int getFd() const { return m_fd; }

    void bind(unsigned short port);
    void listen(int backlog = 1024);
    void connect(struct sockaddr* addr, int length);

    void close() 
    { 
        if (m_fd != INVALD_SOCKET) {
           ::close(m_fd); 
           m_fd = INVALD_SOCKET; 
        }
    }

    int recv(void* buffer, size_t length, int flag);
    int send(const void* buffer, size_t length, int flag);

    void setCloseWait(int delay = 30);
    void setBlock(bool block = false);
    void setTcpNoDelay();
    void setKeepAlive();

    Socket accept(struct sockaddr* addr, int* len);
    
    int getRecvBufferSize() const;
    void setRecvBufferSize(size_t size);
    int getSendBufferSize() const;
    void setSendBufferSize(size_t size);
    
    int setSocketOpt(int level, int opt, const void* pVal, socklen_t len);
    int getSocketOpt(int level, int opt, void* pval, socklen_t* len) const;

private:
    bool m_owner;
    int m_fd;
    int m_domain;
};

}

#endif
