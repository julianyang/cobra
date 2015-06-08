/* 
 * Moudule: core
 * Author: cloudyang@2015/05/15
 */

#ifndef COBRA_CORE_PROTOCOL
#define COBRA_CORE_PROTOCOL

#include <string>
#include <map>

namespace CobraCore {

class Protocol {
public:
    enum ProtocolPaseResult {
        E_PROTOCOL_PARSE_OK = 0,
        E_PROTOCOL_PARSE_LESS = 1,
        E_PROTOCOL_PARSE_ERR = 2
    };

    virtual ~Protocol() {}

    virtual void initialize(const std::string& config) = 0;
    virtual int protocolParse(std::string& buffer, std::string& out) = 0;
    virtual bool canAccept(const std::string& ip) = 0;
    virtual void protocolEncode(std::string& buffer) = 0;

protected:
    Protocol() {}
};

class DefaultProtocol : public Protocol {
public:
    DefaultProtocol(bool filter=false, int maxLength = 100000) 
        : m_reverseFilter(filter)
        , m_maxBufferLength(maxLength) 
    {
    }

    ~DefaultProtocol() {}

    virtual void initialize(const std::string& config);
    virtual int protocolParse(std::string& buffer, std::string& out);
    virtual bool canAccept(const std::string& ip);
    virtual void protocolEncode(std::string& buffer);

private:
    bool m_reverseFilter;
    int m_maxBufferLength;
    std::map<std::string, bool> m_ips;
};

Protocol* createProtocol(int p); 

}

#endif
