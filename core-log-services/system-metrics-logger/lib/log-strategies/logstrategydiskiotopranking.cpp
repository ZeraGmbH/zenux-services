#include "logstrategydiskiotopranking.h"

LogStrategyDiskIoTopRanking::LogStrategyDiskIoTopRanking(int rankingDepth, int measPeriodMs, std::function<void (QString)> loggingFunction) :
    m_rankingDepth(rankingDepth),
    m_measPeriodMs(measPeriodMs),
    m_loggingFunction(loggingFunction)
{
}

void LogStrategyDiskIoTopRanking::addValue(DiskValuesProcesses newValue)
{
    DiskReadWriteRanking ranking = DiskIoToTopRanking::createRanking(newValue);

    QString processesStringRead = genProcessString(ranking.readRanking, ranking.totalBytesRead, m_rankingDepth, m_measPeriodMs);
    if(!processesStringRead.isEmpty())
        m_loggingFunction("Read ranking:" + processesStringRead);

    QString processesStringWrite = genProcessString(ranking.writeRanking, ranking.totalBytesWritten, m_rankingDepth, m_measPeriodMs);
    if(!processesStringWrite.isEmpty())
        m_loggingFunction("Write ranking:" + processesStringWrite);
}

QString LogStrategyDiskIoTopRanking::genProcessString(const QVector<DiskIoRankingEntry>& processRankings,
                                                      const quint64 &totalIo,
                                                      int rankingDepth,
                                                      int measPeriodMs)
{
    QString processesString;
    float measPeriodS = float(measPeriodMs) / 1000.0;
    for(int i=0; i<processRankings.count(); i++) {
        if(i >= rankingDepth)
            break;
        DiskIoRankingEntry rankingEntry = processRankings[i];
        float ioKiBytes = float(rankingEntry.ioBytes) / 1024.0;
        float ioKiBytesPerSecond = ioKiBytes / measPeriodS;
        float percentage = float(rankingEntry.ioBytes) / float(totalIo) * 100.0;
        QString processString = QString(" %1(%2KiB/s / %3%)")
                                    .arg(rankingEntry.processName)
                                    .arg(ioKiBytesPerSecond, 0, 'f', 1)
                                    .arg(percentage, 0, 'f', 1);
        if(!processesString.isEmpty())
            processesString += ",";
        processesString += processString;
    }
    return processesString;
}
