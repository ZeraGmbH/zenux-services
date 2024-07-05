#ifndef SYSTEMINFOFILELOCATOR_H
#define SYSTEMINFOFILELOCATOR_H

#include <QString>

class SystemInfoFileLocator
{
public:
    static QString getProcStatusFileName();
    static QString getProcMeminfoFileName();
    static QString getSysTempRootPath();
protected:
    static QString m_procStatusFileName;
    static QString m_procMeminfoFileName;
    static QString m_sysTempRootPath;
};

#endif // SYSTEMINFOFILELOCATOR_H
