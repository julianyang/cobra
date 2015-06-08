#include <iostream>
#include <string>
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <memory.h>

#include "util/Socket.h"


namespace SocketStream {

class SocketStreamBuffer : public std::streambuf {
public:
    SocketStreamBuffer(const CobraUtil::Socket& socket)
        : m_socket() 
        , m_buffer(new char[buffer_size])
    {
        m_socket.init(socket.getFd(), true);
        setp(m_buffer, m_buffer+buffer_size);
    }

    ~SocketStreamBuffer() { sync(); }

    /*std::char_traits<char>::int_type overflow(std::char_traits<char>::int_type c)
    {
        if (c != EOF) {
            if (m_socket.send(&c, sizeof(std::char_traits<char>::int_type), 0) <= 0)
                return EOF;
        }
        
        return c;
    }*/

    std::char_traits<char>::int_type overflow(std::char_traits<char>::int_type c)
    {
        if (c != EOF) {
            //*pptr() = c;
            //pbump(1);
            sputc(c);
        } else 
            sync();
        
        return c;
    }

    int sync() 
    {
        if (pptr() > pbase()) {
            int size = m_socket.send(m_buffer, pptr()-pbase(), 0);
            setp(pbase(), epptr());
            return size;
        }

        return 0;
    }

private:
    const static int buffer_size = 256;
    CobraUtil::Socket m_socket;
    char *m_buffer;
};

}


int main()
{
    CobraUtil::Socket socket;
    socket.create();
    socket.setOwner(false);

    struct sockaddr_in addr;
    memset(reinterpret_cast<void*>(&addr), 0x00, sizeof(struct sockaddr_in));

    addr.sin_port = htons(10085);
    addr.sin_family= AF_INET;

    std::string name = "10.12.22.13";

    struct in_addr ip;
    struct hostent *hostp = NULL;

    if (inet_aton(name.c_str(), &ip) != 0)
        hostp = gethostbyaddr(reinterpret_cast<const char*>(&ip), sizeof(ip), AF_INET);
    else
        hostp = gethostbyname(name.c_str());

    if (!hostp) { 
        std::cout << "Can not resolve host|name:" << name << std::endl;
        return 0;
    }

    memcpy(reinterpret_cast<char*>(&(addr.sin_addr.s_addr)), reinterpret_cast<char*>(hostp->h_addr), hostp->h_length);

    try {
        socket.connect(reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));
    } catch (CobraUtil::UtilException& e) {
        std::cout << "exception:" << e.what() << std::endl;
        return 0;
    }

    SocketStream::SocketStreamBuffer sbuffer(socket);
    std::ostream output(&sbuffer);

    std::string m = "send test text to server.";
    output << m;

    return 0;
}
