#include "procdiskstatdecoder.h"
#include "systeminfofilelocator.h"
#include <QFile>
#include <regex>

QStringList ProcDiskStatDecoder::getAllDiskBlockDevices()
{
    const QStringList lines = getProcDiskStat().split("\n", Qt::SkipEmptyParts);
    QStringList blkDevices;
    for(const QString &line : lines) {
        QStringList entries = line.split(" ", Qt::SkipEmptyParts);
        if(entries.size() >= 3)
            blkDevices.append(entries[2]);
    }
    return blkDevices;
}

QStringList ProcDiskStatDecoder::getDiskBlockDevicesOfInterest()
{
    const QStringList allDevices = getAllDiskBlockDevices();
    QStringList diskDevices;
    std::regex diskPattern("^sd[a-z]$|^mmcblk[0-9]$");
    for(const QString &device : allDevices) {
        if(std::regex_match(device.toStdString(), diskPattern))
            diskDevices.append(device);
    }
    return diskDevices;
}

DiskValues ProcDiskStatDecoder::getReadWriteBytes(QString device)
{
    const QStringList lines = getProcDiskStat().split("\n", Qt::SkipEmptyParts);
    DiskValues values;
    for(const QString &line : lines) {
        QStringList entries = line.split(" ", Qt::SkipEmptyParts);
        if(entries.size() >= 3) {
            QString currDevice = entries[2];
            if(device == currDevice) {
                values = decodeSingleDiskStatLine(line);
                break;
            }
        }
    }
    return values;
}

DiskValues ProcDiskStatDecoder::decodeSingleDiskStatLine(QString line)
{
    QStringList entries = line.split(" ", Qt::SkipEmptyParts);
    DiskValues values;
    bool ok = false;
    if(entries.size() >= 6) {
        quint64 val = entries[5].toULongLong(&ok);
        if(ok)
            values.bytesRead = val*sectorSizeAllDevices;
    }
    if(entries.size() >= 10) {
        quint64 val = entries[9].toULongLong(&ok);
        if(ok)
            values.bytesWritten = val*sectorSizeAllDevices;
    }
    return values;
}

QString ProcDiskStatDecoder::getProcDiskStat()
{
    QFile file(SystemInfoFileLocator::getProcDiskStatsName());
    QString procDiskStat;
    if(file.open(QFile::ReadOnly))
        procDiskStat = file.readAll();
    else
        qWarning("Cannot read diskstats file");
    return procDiskStat;
}
