/* 
 * Moudule: core
 * Author: cloudyang@2015/05/27
 */

#ifndef COBRA_CORE_LOGGER
#define COBRA_CORE_LOGGER

#include "base/Logger.h"
#include "base/Singleton.h"

namespace CobraCore {

class CoreLogger : public base::Singleton<CoreLogger> {
public:
    CoreLogger() : m_logger() {}
    const base::Logger& logger() const { return m_logger; }

private:
    base::Logger m_logger;
};

#define CORE_DEBUG CoreLogger::getInstance()->logger().debug()
#define CORE_ERROR CoreLogger::getInstance()->logger().error()
#define CORE_INFO CoreLogger::getInstance()->logger().info()
#define CORE_WARN CoreLogger::getInstance()->logger().warn()

}

#endif
