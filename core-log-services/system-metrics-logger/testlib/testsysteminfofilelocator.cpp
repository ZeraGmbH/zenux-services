#include "testsysteminfofilelocator.h"

void TestSystemInfoFileLocator::setProcStatusFileName(QString procStatusFileName)
{
    m_procStatusFileName = procStatusFileName;
}

void TestSystemInfoFileLocator::setSysTempRootPath(QString sysTempRootPath)
{
    m_sysTempRootPath = sysTempRootPath;
}

void TestSystemInfoFileLocator::setSysCpuPath(QString sysCpuPath)
{
    m_sysCpuPath = sysCpuPath;
}

void TestSystemInfoFileLocator::setProcBasePath(QString procBasePath)
{
    m_procBasePath = procBasePath;
}

void TestSystemInfoFileLocator::resetSysFreqPaths()
{
    m_sysFreqPaths.clear();
}
