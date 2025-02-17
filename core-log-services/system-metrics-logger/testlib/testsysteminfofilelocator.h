#ifndef TESTSYSTEMINFOFILELOCATOR_H
#define TESTSYSTEMINFOFILELOCATOR_H

#include <systeminfofilelocator.h>

class TestSystemInfoFileLocator : public SystemInfoFileLocator
{
public:
    static void setSysTempRootPath(QString sysTempRootPath);
    static void setSysCpuPath(QString sysCpuPath);
    static void setProcBasePath(QString procBasePath);
    static void resetSysFreqPaths();
};

#endif // TESTSYSTEMINFOFILELOCATOR_H
