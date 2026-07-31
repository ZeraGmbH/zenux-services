#ifndef TESTSYSTEMINFOFILELOCATOR_H
#define TESTSYSTEMINFOFILELOCATOR_H

#include <systeminfofilelocator.h>

class TestSystemInfoFileLocator : public SystemInfoFileLocator
{
public:
    static void setSysTempRootPath(const QString &sysTempRootPath);
    static void setSysCpuPath(const QString &sysCpuPath);
    static void setProcBasePath(const QString &procBasePath);
    static void resetSysFreqPaths();
};

#endif // TESTSYSTEMINFOFILELOCATOR_H
