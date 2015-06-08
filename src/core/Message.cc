/* 
 * Moudule: core
 * Author: cloudyang@2015/05/20
 */


#include "core/Message.h"

#include "core/ClientConnection.h"
#include "core/CoreLogger.h"

namespace CobraCore {

SendMessage::SendMessage(int type, 
        int timeout, 
        base::RefPtr<RequestCallback>& cb,
        base::RefPtr<ClientConnection>& conn)
    : m_id(0)
    , m_type(type)
    , m_timeout(timeout)
    , m_result(0)
    , m_event()
    , m_conn(conn)
    , m_callback(cb)
{
    if (m_type == ASYNC && !m_callback)
        m_type = ONEWAY;

    CORE_DEBUG << "Create messge!"  << std::endl;
}

SendMessage::~SendMessage() 
{
    CORE_DEBUG << "Destroy messge!"  << std::endl;
}

void SendMessage::onException(int error)
{
    m_request = error;
    if (m_type == ONEWAY)
        return;

    if (m_type == SYNC) {
        notify();
        return;
    }

   // async message.
   ClientConnection::doException(m_conn, error);
}

}
