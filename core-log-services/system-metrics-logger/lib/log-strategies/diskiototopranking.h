#ifndef DISKIOTOTOPRANKING_H
#define DISKIOTOTOPRANKING_H

#include "diskreadwritevalues.h"
#include <QString>
#include <QVector>

struct DiskIoRankingEntry
{
    QString processName;
    quint64 ioBytes = 0;
};

struct DiskReadWriteRanking
{
    quint64 totalBytesRead = 0;
    quint64 totalBytesWritten = 0;
    QVector<DiskIoRankingEntry> readRanking;
    QVector<DiskIoRankingEntry> writeRanking;
};

class DiskIoToTopRanking
{
public:
    static DiskReadWriteRanking createRanking(const DiskValuesProcesses &procDiskValues);
private:
    static void sortAndCalcTotals(const DiskValuesProcesses &procDiskValues,
                                  DiskReadWriteRanking &ranking,
                                  QMap<quint64, QStringList> &sortedProcessesRead,
                                  QMap<quint64, QStringList> &sortedProcessesWritten);
    static void fillRankingLists(DiskReadWriteRanking &ranking,
                            QMap<quint64, QStringList> &sortedProcessesRead,
                            QMap<quint64, QStringList> &sortedProcessesWritten);
};

#endif // DISKIOTOTOPRANKING_H
