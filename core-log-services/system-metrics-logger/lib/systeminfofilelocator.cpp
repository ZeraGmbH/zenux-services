#include "systeminfofilelocator.h"

QString SystemInfoFileLocator::m_procStatusFileName = QStringLiteral("/proc/stat");
QString SystemInfoFileLocator::m_procMeminfoFileName = QStringLiteral("/proc/meminfo");
QString SystemInfoFileLocator::m_sysTempRootPath = QStringLiteral("/sys/class/thermal");

QString SystemInfoFileLocator::getProcStatusFileName()
{
    return m_procStatusFileName;
}

QString SystemInfoFileLocator::getProcMeminfoFileName()
{
    return m_procMeminfoFileName;
}

QString SystemInfoFileLocator::getSysTempRootPath()
{
    return m_sysTempRootPath;
}
