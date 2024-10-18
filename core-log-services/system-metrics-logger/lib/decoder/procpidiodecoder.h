#ifndef PROCPIDIODECODER_H
#define PROCPIDIODECODER_H

#include "diskreadwritevalues.h"
#include <QMap>
#include <QVector>

class ProcPidIoDecoder
{
public:
    static DiskValues getReadWrites(int pid);
    static QMap<int, DiskValues> getPidGroupReadWrites(QVector<int> pids);
};

#endif // PROCPIDIODECODER_H
