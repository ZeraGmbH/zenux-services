#include "procpiddetails.h"
#include "systeminfofilelocator.h"
#include <QFile>

QString ProcPidDetails::getProcesName(int pid)
{
    QString processName;
    const QString fileName = SystemInfoFileLocator::getProcFileName(pid, "comm");
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        processName = file.readLine().trimmed();
        file.close();
    }
    return processName;
}
