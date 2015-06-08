#include "util/Socket.h"

#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include "util/UtilTools.h"

namespace CobraUtil {

void Socket::init(int fd, bool owner, int domain)
{
    if (m_owner)
        close();

    m_fd = fd;
    m_owner = owner;
    m_domain = domain;
}

void Socket::create(int domain, int type, int protocol) 
{
    // domain must be AF_INET, AF_UNIX or AF_LOCAL
    // FixMe: add assert here.
    if (m_owner)
        close();

    m_domain = domain;
    m_fd = socket(domain, type, 0);

    if (m_fd == INVALD_SOCKET) {
        std::string m = "Create soket error:" + std::string(strerror(errno));
        throw SocketException(m);
    }
}

void Socket::bind(unsigned short port) 
{
    struct sockaddr_in addr;
    bzero(reinterpret_cast<char*>(&addr), sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    int val = 1; // reuse addr.
    setSocketOpt(SO_REUSEADDR, SOL_SOCKET, reinterpret_cast<const void *>(&val), sizeof(int));
    
    if (::bind(m_fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::string m = "bind error:" + std::string(strerror(errno)) + "|prot:" + UtilTools::toString(port);
        throw SocketException(m); 
    }
}

void Socket::listen(int backlog) 
{
    if (::listen(m_fd, backlog) < 0) {
        std::string m = "listen error:" + std::string(strerror(errno));
        throw SocketException(m);
    }
}

void Socket::connect(struct sockaddr* addr, int length)
{
    if (::connect(m_fd, addr, length) < 0) {
        std::string m = "connect error:" + std::string(strerror(errno));
        throw SocketException(m);
    }
}

Socket Socket::accept(struct sockaddr* addr, int* len)
{
    int fd = ::accept(m_fd, addr, reinterpret_cast<socklen_t*>(len));
    if (fd < 0)
        return Socket(-1);
    return Socket(fd);
}

int Socket::recv(void* buffer, size_t length, int flag)
{
    return ::recv(m_fd, buffer, length, flag);
}

int Socket::send(const void* buffer, size_t length, int flag)
{
    return ::send(m_fd, buffer, length, flag);
}

void Socket::setCloseWait(int delay)
{
    linger stLinger;
    stLinger.l_onoff = 1;
    stLinger.l_linger = delay;
    if (setSocketOpt(SOL_SOCKET, SO_LINGER, reinterpret_cast<const void*>(&stLinger), sizeof(stLinger)) < 0) {
        std::string m = "set SO_LINGER error:" + std::string(strerror(errno));
        throw SocketException(m);
    }
}

void Socket::setBlock(bool block)
{
    int val = fcntl(m_fd, F_GETFL, 0);
    if (val == -1) {
        std::string m = "fcntl get error:" + std::string(strerror(errno)); 
        throw SocketException(m); 
    }

    if (!block)
        val |= O_NONBLOCK; 
    else 
        val &= ~O_NONBLOCK;

    if (fcntl(m_fd, F_SETFL, val) == -1) {
        std::string m = "fcntl set error:" + std::string(strerror(errno)); 
        throw SocketException(m); 
    }
}

void Socket::setTcpNoDelay() 
{
    int flag = 1;
    if (setSocketOpt(IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const void*>(&flag), sizeof(flag)) < 0) {
        std::string m = "set TCP_NODELAY error:" + std::string(strerror(errno));
        throw SocketException(m);
    }
}

void Socket::setKeepAlive()
{
    int flag = 1;
    if (setSocketOpt(SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<const void*>(&flag), sizeof(flag)) < 0) {
        std::string m = "set SO_KEEPALIVE error:" + std::string(strerror(errno));
        throw SocketException(m);
    }
}

int Socket::getRecvBufferSize() const 
{
    int val;
    socklen_t length = sizeof(val);
    if (getSocketOpt(SOL_SOCKET, SO_RCVBUF, reinterpret_cast<void*>(&val), &length) < 0) {
        std::string m = "get SO_RCVBUF error:" + std::string(strerror(errno));
        throw SocketException(m);
    }

    return val;
}

void Socket::setRecvBufferSize(size_t size)
{
    if (setSocketOpt(SOL_SOCKET, SO_RCVBUF, reinterpret_cast<void*>(&size), sizeof(size)) < 0) {
        std::string m = "set SO_RCVBUF error:" + std::string(strerror(errno));
        throw SocketException(m);
    }
}

int Socket::getSendBufferSize() const 
{
    int val;
    socklen_t length = sizeof(val);
    if (getSocketOpt(SOL_SOCKET, SO_SNDBUF, reinterpret_cast<void*>(&val), &length) < 0) {
        std::string m = "get SO_SNDBUF error:" + std::string(strerror(errno));
        throw SocketException(m);
    }

    return val;
}

void Socket::setSendBufferSize(size_t size) 
{
    if (setSocketOpt(SOL_SOCKET, SO_SNDBUF, reinterpret_cast<void*>(&size), sizeof(size)) < 0) {
        std::string m = "set SO_SNDBUF error:" + std::string(strerror(errno));
        throw SocketException(m);
    }

}

int Socket::setSocketOpt(int level, int opt, const void* pVal, socklen_t len)
{
    return ::setsockopt(m_fd, level, opt, pVal, len);
}

int Socket::getSocketOpt(int level, int opt, void* pVal, socklen_t* len) const
{
    return ::getsockopt(m_fd, level, opt, pVal, len);
}

}
