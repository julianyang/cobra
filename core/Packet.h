/* 
 * Moudule: core
 * Author: cloudyang@2015/04/20
 */

#ifndef COBRA_CORE_PACKET
#define COBRA_CORE_PACKET

#include <string>

namespace CobraCore {

class RecvPacket {
public:
    RecvPacket()
        : m_id(-1)
        , m_fd(-1)
        , m_port(0)
        , m_timeout(60000)
        , m_recvTime(0)
    {
    }

    int m_id; // connection id
    int m_fd; // connection socket.
    int m_port; // peer port.
    int m_timeout; // timeout.
    long long m_recvTime; // recv time. ms
    std::string m_ip; // peer id(10.10.10.10).
    std::string m_buffer; // recv data(protocol data).
};

class SendPacket {
public:
    SendPacket()
        : m_id(-1)
        , m_port(0)
        , m_cmd('s')
    {
    }

    int m_id; // connection id.
    int m_port; // peer port.
    char m_cmd; // 'c' for close, 's' for send.
    std::string m_ip; //peer ip.
    std::string m_buffer; // send data(protocol data).
};

}

#endif
