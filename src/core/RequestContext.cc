/* 
 * Moudule: core
 * Author: cloudyang@2015/04/20
 */

#include "core/RequestContext.h"

#include <assert.h>
#include "core/Packet.h"
#include "core/RequestHandleThread.h"
#include "core/CoreLogger.h"

namespace CobraCore {

RequestContext::RequestContext(const RecvPacket* p, RequestHandleThread* thread)
    : m_response(false)
    , m_id(p->m_id)
    , m_fd(p->m_fd)
    , m_port(p->m_port)
    , m_timeout(p->m_timeout)
    , m_recvTime(p->m_recvTime)
    , m_handleThead(thread)
    , m_ip(p->m_ip)
    , m_buffer(p->m_buffer)
{
    assert(m_handleThead);

    CORE_DEBUG << "Create RequestContext|id:" << m_id
        << "|fd:" << m_fd
        << "|ip:" << m_ip
        << "|port:" << m_port
        << "|recvTime:" << m_recvTime
        << std::endl;
}

RequestContext::~RequestContext()
{
    CORE_DEBUG << "Destroy RequestContext|id:" << m_id
        << "|fd:" << m_fd
        << "|ip:" << m_ip
        << "|port:" << m_port
        << "|recvTime:" << m_recvTime
        << std::endl;
}

void RequestContext::close() 
{
    m_handleThead->closeConnection(m_id, m_fd);
}

void RequestContext::sendResponse(const std::string& buffer)
{
    SendPacket* send = new SendPacket;
    send->m_id = m_id;
    send->m_port = m_port;
    send->m_cmd = 's';
    send->m_ip = m_ip;
    send->m_buffer = buffer;

    m_handleThead->sendResponse(m_fd, send);
}

}
