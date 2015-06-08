/* 
 * Moudule: core
 * Author: cloudyang@2015/05/16
 */

#include "core/RequestHandleThread.h"

#include <assert.h>
#include "base/RefPtr.h"
#include "core/Packet.h"
#include "core/RequestHandle.h"
#include "core/RequestContext.h"
#include "core/CoreLogger.h"

namespace CobraCore {

RequestHandleThread::RequestHandleThread(RequestHandleThreadClient* client)
    : OneWorkThreadProxy("WorkerThread", 50)
    , m_client(client)
    , m_handle(NULL)
{
    assert(m_client);

    CORE_DEBUG << "Create RequestHandleThread" << std::endl;
}

RequestHandleThread::~RequestHandleThread()
{
    CORE_DEBUG << "Destroy RequestHandleThread" << std::endl;
}

void RequestHandleThread::doWork() 
{
    assert(m_handle);

    RecvPacket* p = NULL;
    if (!m_client->receivePacket(p, 100) || !p)
        return;

    base::RefPtr<RequestContext> c = new RequestContext(p, this);
    m_handle->onRequest(c);
    delete p;
}

void RequestHandleThread::addRequestHandle(RequestHandle* handle) 
{
    m_handle = handle;
}

}
