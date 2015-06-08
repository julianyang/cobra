/* 
 * Moudule: core
 * Author: cloudyang@2015/04/17
 */

#ifndef COBRA_CORE_CLIENTPROXY
#define COBRA_CORE_CLIENTPROXY

#include <string>
#include "base/RefCountedThreadSafe.h"
#include "base/RefPtr.h"

namespace CobraCore {

class ClientProxy : public base::RefCounteddThreadSafe<ClientProxy> {
public:
    class ClientProxyCallback : public base::RefCountedThreadSafe<ClientProxyCallback> {
    public:
        virtual ~ClientProxyCallback() { }
        virtual void onResponse(int ret, const std::string& buffer) = 0;
        virtual void onException(const std::string& message, int code) = 0;

    protected:
        ClientProxyCallback() { }
        friend class base::RefCountedThreadSafe<ClientProxyCallback>;
    };

    virtual ~ClientProxy() { }
    
    virtual bool encodePacket(std::string& packet);
    virtual bool decodePacket(std::string& packet);
    virtual int doRequest(const std::string& request, std::string& response)  = 0;
    virtual void async_doRequest(base::RefPtr<ClientProxyCallback> cb, const std::string& request)  = 0;

protected:
    friend class base::RefCountedThreadSafe<ClientProxy>;
};

}

#endif
