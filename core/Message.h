/* 
 * Moudule: core
 * Author: cloudyang@2015/05/20
 */

#ifndef COBRA_CORE_MESSAGE
#define COBRA_CORE_MESSAGE

#include <string>
#include "base/WaitableEvent.h"
#include "base/RefPtr.h"
#include "core/RequestCallback.h"

namespace CobraCore {

class ClientConnection;

class SendMessage {
public:
    enum MessageType {
        SYNC = 0,
        ASYNC,
        ONEWAY
    };

    enum ExceptionType {
        E_QUEUE_FULL = -1,
        E_SEND_ERROR = -2,
        E_RECV_ERROR = -3,
        E_SOCKET_ERROR = -4,
        E_TIMEOUT = -10
    };

    SendMessage(int type, int timeout, base::RefPtr<RequestCallback>& cb, base::RefPtr<ClientConnection>& conn);
    ~SendMessage();

    void onException(int error /*ExceptionType*/);

    void setId(int uid) { m_id = uid; }
    int getId() const { return m_id; }
    
    void setTimeout(int timeout /*ms*/) { m_timeout = timeout; }
    int getTimeout() const { return m_timeout; }

    int getType() const { return m_type; }
    
    void setRequestBuffer(const std::string& buffer) { m_request = buffer; }
    const std::string& getRequestBuffer() const { return m_request; }
    
    void setResponseBuffer(const std::string& buffer) { m_response = buffer; }
    const std::string& getResponseBuffer() const { return m_response; }

    void wait() { m_event.wait(); }
    void notify() { m_event.notify(); }

    void setResult(int code) { m_result = code; }
    int getResult() const { return m_result; }

    ClientConnection* getConnection() { return m_conn.get(); }
    RequestCallback* getCallback() { return m_callback.get(); }

private:
    int m_id;
    int m_type;
    int m_timeout;
    int m_result;

    std::string m_request; // request buffer.
    std::string m_response; // response buffer.

    base::WaitableEvent m_event; // sync wait event.
    base::RefPtr<ClientConnection> m_conn; // connection;
    base::RefPtr<RequestCallback> m_callback; // async callback object.
};

}

#endif
