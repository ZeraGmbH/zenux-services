#include "procpiddetails.h"
#include "systeminfofilelocator.h"
#include <QFile>
#include <QTextStream>

QString ProcPidDetails::getProcesName(int pid)
{
    QString processName;
    QString procDir = SystemInfoFileLocator::getProcBasePath();
    if(!procDir.endsWith("/"))
        procDir += "/";
    const QString fileName = QString("%1%2/comm").arg(procDir).arg(pid);
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        processName = file.readLine().trimmed();
        file.close();
    }
    return processName;
}
