#ifndef SYSTEMINFOFILELOCATOR_H
#define SYSTEMINFOFILELOCATOR_H

#include <QString>
#include <QList>

class SystemInfoFileLocator
{
public:
    static QString getProcStatusFileName();
    static QString getProcMeminfoFileName();
    static QString getSysTempRootPath();
    static QList<QString> getSysFreqPaths();
protected:
    static QString m_procStatusFileName;
    static QString m_procMeminfoFileName;
    static QString m_sysTempRootPath;
    static QList<QString> m_sysFreqPaths;
    static QString m_sysCpuPath;
};

#endif // SYSTEMINFOFILELOCATOR_H
