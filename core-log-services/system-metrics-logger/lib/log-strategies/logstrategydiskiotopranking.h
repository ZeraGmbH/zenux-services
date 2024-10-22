#ifndef LOGSTRATEGYDISKIOTOPRANKING_H
#define LOGSTRATEGYDISKIOTOPRANKING_H

#include "abstractlogstrategy.h"
#include "diskiototopranking.h"
#include "diskreadwritevalues.h"

class LogStrategyDiskIoTopRanking : public AbstractLogStrategy<DiskValuesProcesses>
{
public:
    LogStrategyDiskIoTopRanking(int rankingDepth, int measPeriodMs, std::function<void(QString)> loggingFunction = [](QString logString){
        qInfo("%s",qPrintable(logString));
    });
    void addValue(DiskValuesProcesses newValue) override;
private:
    static QString genProcessString(const QVector<DiskIoRankingEntry> &processRankings,
                                    const quint64 &totalIo,
                                    int rankingDepth,
                                    int measPeriodMs);
    int m_rankingDepth;
    int m_measPeriodMs;
    std::function<void(QString)> m_loggingFunction;
};

#endif // LOGSTRATEGYDISKIOTOPRANKING_H
