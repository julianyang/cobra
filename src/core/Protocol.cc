/* 
 * Moudule: core
 * Author: cloudyang@2015/05/15
 */


#include "core/Protocol.h"

#include <memory.h>
#include <arpa/inet.h>

namespace CobraCore {

Protocol* createProtocol(int p) 
{
    switch (p) {
    case 0:
    default:
        return new DefaultProtocol();
        break;
    }
}

void DefaultProtocol::initialize(const std::string& config)
{

}

int DefaultProtocol::protocolParse(std::string& buffer, std::string& out)
{
    // default packet is length + data.
    // sizeof(length) == sizeof(int).
    int lengthBytes = sizeof(int);
    if (buffer.length() < static_cast<size_t>(lengthBytes))
        return E_PROTOCOL_PARSE_LESS;

    int lengthBUffer;
    memcpy(&lengthBUffer, buffer.c_str(), lengthBytes);

    int length = ntohl(lengthBUffer);
    if (length < lengthBytes || length > m_maxBufferLength)
        return E_PROTOCOL_PARSE_ERR;

    if (static_cast<size_t>(length) > buffer.length())
        return E_PROTOCOL_PARSE_LESS;

    out = buffer.substr(lengthBytes, length-lengthBytes);
    buffer = buffer.substr(length);

    return E_PROTOCOL_PARSE_OK;
}

bool DefaultProtocol::canAccept(const std::string& ip)
{
    if (m_reverseFilter)
        return m_ips.find(ip) != m_ips.end();
    else 
        return m_ips.find(ip) == m_ips.end(); 
}

void DefaultProtocol::protocolEncode(std::string& buffer) 
{
    int length = buffer.length() + sizeof(int);
    length = htonl(length);
    std::string s;
    s.append(reinterpret_cast<const char*>(&length), sizeof(int));
    buffer = s + buffer;
}

}
