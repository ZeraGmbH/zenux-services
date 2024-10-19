#ifndef PROCPIDIODECODER_H
#define PROCPIDIODECODER_H

#include "diskreadwritevalues.h"
#include <QVector>

class ProcPidIoDecoder
{
public:
    static DiskValues getReadWrites(int pid);
    static DiskValuesProcesses getPidGroupReadWrites(QVector<int> pids);
};

#endif // PROCPIDIODECODER_H
