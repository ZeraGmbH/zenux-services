#ifndef SYSTEMINFOFILELOCATOR_H
#define SYSTEMINFOFILELOCATOR_H

#include <QString>
#include <QList>

class SystemInfoFileLocator
{
public:
    static QString getProcStatusFileName();
    static QString getSysTempRootPath();
    static QList<QString> getSysFreqPaths();
    static QString getProcBasePath();
    static const QString getProcFileName(const QString &fileName);
    static const QString getProcFileName(int pid, const QString &fileName);
protected:
    static QString m_procStatusFileName;
    static QString m_sysTempRootPath;
    static QList<QString> m_sysFreqPaths;
    static QString m_sysCpuPath;
    static QString m_procBasePath;
};

#endif // SYSTEMINFOFILELOCATOR_H
