#ifndef LOGSTRATEGYDISKIOTOPRANKING_H
#define LOGSTRATEGYDISKIOTOPRANKING_H

#include "abstractlogstrategy.h"
#include "diskiototopranking.h"
#include "diskreadwritevalues.h"

class LogStrategyDiskIoTopRanking : public AbstractLogStrategy<DiskValuesProcesses>
{
public:
    LogStrategyDiskIoTopRanking(int rankingDepth, std::function<void(QString)> loggingFunction = [](QString logString){
        qInfo("%s",qPrintable(logString));
    });
    void addValue(DiskValuesProcesses newValue) override;
private:
    QString genProcessString(const QVector<DiskIoRankingEntry> &processRankings, const quint64 &totalIo);
    int m_rankingDepth;
    std::function<void(QString)> m_loggingFunction;
};

#endif // LOGSTRATEGYDISKIOTOPRANKING_H
