/* 
 * Moudule: core
 * Author: cloudyang@2015/05/20
 */

#ifndef COBRA_CORE_REQUSTCALLBACK
#define COBRA_CORE_REQUSTCALLBACK

#include <string>
#include "base/RefCountedThreadSafe.h"

namespace CobraCore {

class RequestCallback : public base::RefCountedThreadSafe<RequestCallback> {
public:
    virtual ~RequestCallback() {}

    virtual void onRequestCompleted(int ret, std::string& response) = 0;

protected:
    friend class base::RefCountedThreadSafe<RequestCallback>;
    RequestCallback() { }
};

}

#endif
