#include "diskiototopranking.h"
#include "procpiddetails.h"

DiskReadWriteRanking DiskIoToTopRanking::createRanking(const DiskValuesProcesses &procDiskValues)
{
    QMap<quint64, QStringList> sortedProcessesRead;
    QMap<quint64, QStringList> sortedProcessesWritten;
    DiskReadWriteRanking ranking;
    sortAndCalcTotals(procDiskValues,
                      ranking,
                      sortedProcessesRead,
                      sortedProcessesWritten);

    fillRankingLists(ranking, sortedProcessesRead, sortedProcessesWritten);
    return ranking;
}

void DiskIoToTopRanking::sortAndCalcTotals(const DiskValuesProcesses &procDiskValues,
                                           DiskReadWriteRanking &ranking,
                                           QMap<quint64, QStringList> &sortedProcessesRead,
                                           QMap<quint64, QStringList> &sortedProcessesWritten)
{
    quint64 totalBytesRead = 0;
    quint64 totalBytesWritten = 0;
    for(auto iter=procDiskValues.cbegin(); iter!=procDiskValues.cend(); ++iter) {
        QString processName = ProcPidDetails::getProcesName(iter.key());
        if(!processName.isEmpty()) {
            quint64 bytesRead = iter.value().bytesRead;
            if(bytesRead) {
                totalBytesRead += bytesRead;
                sortedProcessesRead[bytesRead].prepend(processName);
            }
            quint64 bytesWritten = iter.value().bytesWritten;
            if(bytesWritten) {
                totalBytesWritten += bytesWritten;
                sortedProcessesWritten[bytesWritten].prepend(processName);
            }
        }
    }
    ranking.totalBytesRead = totalBytesRead;
    ranking.totalBytesWritten = totalBytesWritten;
}

void DiskIoToTopRanking::fillRankingLists(DiskReadWriteRanking &ranking, QMap<quint64, QStringList> &sortedProcessesRead, QMap<quint64, QStringList> &sortedProcessesWritten)
{
    for(auto iter=sortedProcessesRead.cbegin(); iter!=sortedProcessesRead.cend(); ++iter) {
        const QStringList processNames = iter.value();
        for(const QString &processName : processNames) {
            DiskIoRankingEntry entry = { processName, iter.key() };
            ranking.readRanking.prepend(entry);
        }
    }
    for(auto iter=sortedProcessesWritten.cbegin(); iter!=sortedProcessesWritten.cend(); ++iter) {
        const QStringList processNames = iter.value();
        for(const QString &processName : processNames) {
            DiskIoRankingEntry entry = { processName, iter.key() };
            ranking.writeRanking.prepend(entry);
        }
    }
}
