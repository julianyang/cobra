/* 
 * Moudule: core
 * Author: cloudyang@2015/04/20
 */

#ifndef COBRA_CORE_REQUESTCONTEXT
#define COBRA_CORE_REQUESTCONTEXT

#include <string>
#include "base/RefCountedThreadSafe.h"

namespace CobraCore {

struct RecvPacket;
struct RequestHandleThread;

class RequestContext : public base::RefCountedThreadSafe<RequestContext> {
public:
    explicit RequestContext(const RecvPacket* p, RequestHandleThread* thread);
    ~RequestContext();

    int getId() const { return m_id; }
    int getFd() const { return m_fd; }

    int getPort() const { return m_port; }
    long long getRecvTime() const { return m_recvTime; }
    int getTimeout() const { return m_timeout; }
    std::string getIp() const { return m_ip; }

    void setResponse(bool v = true) { m_response = v; }
    bool isResponsed() const { return m_response; }

    void sendResponse(const std::string& data);
    void close();

    const std::string& getRequestBuffer() const { return m_buffer; }

protected:
    friend class base::RefCountedThreadSafe<RequestContext>;

private:
    bool m_response;
    int m_id;
    int m_fd;
    int m_port;
    int m_timeout;
    long long m_recvTime;
    RequestHandleThread* m_handleThead;
    std::string m_ip;
    std::string m_buffer;
};

}

#endif
