#ifndef TESTSYSTEMINFOFILELOCATOR_H
#define TESTSYSTEMINFOFILELOCATOR_H

#include <systeminfofilelocator.h>

class TestSystemInfoFileLocator : public SystemInfoFileLocator
{
public:
    static void setProcStatusFileName(QString procStatusFileName);
    static void setProcMeminfoFileName(QString procMeminfoFileName);
    static void setSysTempRootPath(QString sysTempRootPath);
};

#endif // TESTSYSTEMINFOFILELOCATOR_H
