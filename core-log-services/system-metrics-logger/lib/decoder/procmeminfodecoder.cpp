#include "procmeminfodecoder.h"
#include "systeminfofilelocator.h"
#include <QFile>

MemoryValues ProcMeminfoDecoder::getCurrentMemoryValues()
{
    const QStringList procMeminfoLines = getProcMeminfo().split("\n");
    MemoryValues currentMemoryValues;
    for(const QString &line : procMeminfoLines) {
        if(line.startsWith("MemTotal"))
            currentMemoryValues.totalMemory = decodeSingleMemoryValue(line);
        if(line.startsWith("MemFree"))
            currentMemoryValues.freeMemory = decodeSingleMemoryValue(line);
        if(line.startsWith("Buffers"))
            currentMemoryValues.buffers = decodeSingleMemoryValue(line);
        if(line.startsWith("Cached"))
            currentMemoryValues.cached = decodeSingleMemoryValue(line);
        if(line.startsWith("SReclaimable"))
            currentMemoryValues.sReclaimable = decodeSingleMemoryValue(line);
        if(line.startsWith("Shmem"))
            currentMemoryValues.shmem = decodeSingleMemoryValue(line);
    }
    return currentMemoryValues;
}

QString ProcMeminfoDecoder::getProcMeminfo()
{
    QFile file(SystemInfoFileLocator::getProcFileName("meminfo"));
    QString procMeminfo;
    if(file.open(QFile::ReadOnly))
        procMeminfo = file.readAll();
    else
        qWarning("Cannot read meminfo file");
    return procMeminfo;
}

quint32 ProcMeminfoDecoder::decodeSingleMemoryValue(QString infoLine)
{
    QStringList parts = infoLine.split(" ", Qt::SkipEmptyParts);
    return parts[1].toUInt();
}
