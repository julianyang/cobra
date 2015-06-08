/* 
 * Moudule: core
 * Author: cloudyang@2015/04/20
 */

#ifndef COBRA_CORE_REQUESTHANDLE
#define COBRA_CORE_REQYESTHANDLE

#include <string>
#include "base/RefPtr.h"

namespace CobraCore {

class RequestContext;

class RequestHandle {
public:
    virtual ~RequestHandle() {}

    virtual void onRequest(base::RefPtr<RequestContext>& context) = 0;

protected:
    RequestHandle() {}
};

}

#endif
