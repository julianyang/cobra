/* 
 * Moudule: core
 * Author: cloudyang@2015/04/17
 */

#include "core/OneWorkThreadProxy.h"

#include "base/RunLoop.h"

namespace CobraCore {

OneWorkThreadProxy::~OneWorkThreadProxy()
{
    if (m_running) {
        m_task->cancel();
        m_workThread.stop();
        m_running = false;
    }
}

void OneWorkThreadProxy::setWorkInterval(int interval) 
{
    m_interval = interval;
    if (m_running) 
        m_task->setInterval(m_interval);
}

void OneWorkThreadProxy::start()
{
    if (m_running)
        return;

    m_running = true;

    m_task = new base::Task<OneWorkThreadProxy>(this, &OneWorkThreadProxy::doWork);
    m_task->setInterval(m_interval);

    m_workThread.start();

    base::RunLoop* workThreadLoop = m_workThread.runLoop();
    workThreadLoop->postTask(m_task.get());
}

void OneWorkThreadProxy::stop()
{
    if (!m_running)
        return;

    m_running = false;
    m_task->cancel();
    m_workThread.stop();
}

}
