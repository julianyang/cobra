/* 
 * Moudule: core
 * Author: cloudyang@2015/04/17
 */

#ifndef COBRA_CORE_NETCONTEXT
#define COBRA_CORE_NETCONTEXT

#include <string>
#include "base/RefCountedThreadSafe.h"

namespace CobraCore {

class RequestHandle;

class NetContext : public base::RefCountedThreadSafe<NetContext> {
public:
    virtual ~NetContext() {}
    
    // config string is formated by json.
    virtual void init(const std::string& config) = 0;
    virtual void addHandle(unsigned short port, RequestHandle* handle)  {}
    virtual void start() = 0;
    virtual void stop() = 0;

protected:
    NetContext() {}
    friend class base::RefCountedThreadSafe<NetContext>;
};

}

#endif
