#ifndef PROCDISKSTATDECODER_H
#define PROCDISKSTATDECODER_H

#include "diskreadwritevalues.h"
#include <QStringList>

class ProcDiskStatDecoder
{
public:
    static QStringList getAllDiskBlockDevices();
    static QStringList getDiskBlockDevicesOfInterest();
    static DiskValues getReadWriteBytes(const QString &device);
    static DiskValues decodeSingleDiskStatLine(const QString &line);
private:
    static QString getProcDiskStat();
};

#endif // PROCDISKSTATDECODER_H
