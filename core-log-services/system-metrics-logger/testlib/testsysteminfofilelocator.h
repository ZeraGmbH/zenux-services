#ifndef TESTSYSTEMINFOFILELOCATOR_H
#define TESTSYSTEMINFOFILELOCATOR_H

#include <systeminfofilelocator.h>

class TestSystemInfoFileLocator : public SystemInfoFileLocator
{
public:
    static void setProcStatusFileName(QString procStatusFileName);
    static void setProcMeminfoFileName(QString procMeminfoFileName);
    static void setProcInterruptFile(QString procInterruptFileName);
    static void setSysTempRootPath(QString sysTempRootPath);
    static void setSysCpuPath(QString sysCpuPath);
    static void setProcDiskStatsName(QString procDiskStatsName);
    static void resetSysFreqPaths();
};

#endif // TESTSYSTEMINFOFILELOCATOR_H
