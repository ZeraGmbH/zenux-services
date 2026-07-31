#include "testsysteminfofilelocator.h"

void TestSystemInfoFileLocator::setSysTempRootPath(const QString &sysTempRootPath)
{
    m_sysTempRootPath = sysTempRootPath;
}

void TestSystemInfoFileLocator::setSysCpuPath(const QString &sysCpuPath)
{
    m_sysCpuPath = sysCpuPath;
}

void TestSystemInfoFileLocator::setProcBasePath(const QString &procBasePath)
{
    m_procBasePath = procBasePath;
}

void TestSystemInfoFileLocator::resetSysFreqPaths()
{
    m_sysFreqPaths.clear();
}
