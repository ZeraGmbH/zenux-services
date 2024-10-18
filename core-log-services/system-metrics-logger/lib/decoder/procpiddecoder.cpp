#include "procpiddecoder.h"
#include "systeminfofilelocator.h"
#include <QDir>

QVector<int> ProcPidDecoder::getPids()
{
    QVector<int> pids;
    QDir procDir(SystemInfoFileLocator::getProcBasePath());
    const QStringList entries = procDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString &entry : entries) {
        bool ok;
        int pid = entry.toInt(&ok);
        if(ok)
            pids.append(pid);
    }
    return pids;
}
