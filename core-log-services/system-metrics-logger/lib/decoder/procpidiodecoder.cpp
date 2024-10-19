#include "procpidiodecoder.h"
#include "systeminfofilelocator.h"
#include <QFile>

DiskValues ProcPidIoDecoder::getReadWrites(int pid)
{
    DiskValues values;
    QString procDir = SystemInfoFileLocator::getProcBasePath();
    if(!procDir.endsWith("/"))
        procDir += "/";
    const QString fileName = QString("%1%2/io").arg(procDir).arg(pid);
    QFile ioFile(fileName);
    if(ioFile.open(QFile::ReadOnly)) {
        const QString contents = ioFile.readAll();
        const QStringList lines = contents.split("\n");
        for(const auto &line : lines) {
            const QStringList lineParts = line.split(" ", Qt::SkipEmptyParts);
            if(lineParts.size() == 2) {
                if(lineParts[0] == "read_bytes:") {
                    bool ok = false;
                    quint64 val = lineParts[1].toULongLong(&ok);
                    if(ok)
                        values.bytesRead = val;
                }
                else if(lineParts[0] == "write_bytes:") {
                    bool ok = false;
                    quint64 val = lineParts[1].toULongLong(&ok);
                    if(ok)
                        values.bytesWritten = val;
                }
            }
        }
    }
    return values;
}

DiskValuesProcesses ProcPidIoDecoder::getPidGroupReadWrites(QVector<int> pids)
{
    DiskValuesProcesses processesValues;
    for(int pid : pids) {
        DiskValues values = getReadWrites(pid);
        if(values.bytesRead > 0 || values.bytesWritten > 0)
            processesValues[pid] = values;
    }
    return processesValues;
}
