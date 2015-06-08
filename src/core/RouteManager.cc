/* 
 * Moudule: core
 * Author: cloudyang@2015/05/20
 */

#include "core/RouteManager.h"

#include "base/Time.h" 
#include "util/UtilTools.h"

namespace CobraCore {

RouteManager::RouteManager()
    : m_initialize(false) 
    , m_policy(0)
    , m_updateInterval(60000) // 1 minute
    , m_registerIp()
    , m_registerPort(0)
    , m_registerSocket()
    , m_cacheLock()
{

}

RouteManager::~RouteManager() 
{
    // 1. stop update thread
    m_registerSocket.close();
    m_cache.clear();
}

void RouteManager::initialize(const std::string& config)
{
    if (m_initialize)
        return;

    m_initialize = true;

    // 1. parse config here.

    m_registerIp = "127.0.0.1";
    m_registerPort = 12345;

    // 2. creae soket and connect to register host.
    //m_registerSocket.create();
    
    // add debug info.
    std::vector<std::string> ipPorts;
    ipPorts.push_back("10.12.22.13:10085");
    std::map<int, std::vector<std::string> > module;
    module[0] = ipPorts;

    m_cache[1] = module;
}

bool RouteManager::getRouteInfo(RouteInfo& info) const
{
    int factor = base::Time::now().toInternalValue()/base::Time::kMicrosecondsPerMillisecond;
    std::string ipPort;
    {
        base::AutoLock lock(const_cast<base::Lock&>(m_cacheLock));
        std::map<int, std::map<int, std::vector<std::string> > >::const_iterator it = m_cache.find(info.m_module);
        if (it == m_cache.end())
            return false;
        std::map<int, std::vector<std::string> >::const_iterator iter = (it->second).find(info.m_set);
        if (iter == (it->second).end() || (iter->second).size() <= 0)
            return false;
        ipPort = (iter->second)[factor%((iter->second).size())];
    }
    
    size_t pos = ipPort.find(":");
    if (pos == std::string::npos)
        return false;
    
    info.m_hostIp = ipPort.substr(0, pos);
    info.m_hostPort = CobraUtil::UtilTools::stringTo<unsigned short>(ipPort.substr(pos+1));

    return true;
}

void RouteManager::reportRouteInfo(const RouteInfo& info) 
{
    // not implementation
}

void RouteManager::updateRouteInfo()
{
    // not implementation
}

}
