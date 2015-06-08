/* 
 * Moudule: core
 * Author: cloudyang@2015/05/20
 */

#ifndef COBRA_CORE_ROUTEMANAGER
#define COBRA_CORE_ROUTEMANAGER

#include <string>
#include <vector>
#include <map>
#include "base/Lock.h"
#include "base/Singleton.h"
#include "util/Socket.h"

namespace CobraCore {

class RouteManager : public base::Singleton<RouteManager> {
public:
    class RouteInfo {
    public:
        int m_module;
        int m_set;
        std::string m_hostIp;
        unsigned short m_hostPort;
        int m_result;
        int m_interval;
    };

    RouteManager();
    ~RouteManager();

    void initialize(const std::string& config);
    bool getRouteInfo(RouteInfo& info) const;
    void reportRouteInfo(const RouteInfo& info);

private:
    void updateRouteInfo();

    bool m_initialize;
    int m_policy;
    int m_updateInterval;

    std::string m_registerIp;
    unsigned short m_registerPort;
    CobraUtil::Socket m_registerSocket;

    base::Lock m_cacheLock;
    std::map<int, std::map<int, std::vector<std::string> > > m_cache;
};

}

#endif
