#include "logstrategydiskiotopranking.h"

LogStrategyDiskIoTopRanking::LogStrategyDiskIoTopRanking(int rankingDepth, std::function<void (QString)> loggingFunction) :
    m_rankingDepth(rankingDepth),
    m_loggingFunction(loggingFunction)
{
}

void LogStrategyDiskIoTopRanking::addValue(DiskValuesProcesses newValue)
{
    int processCount = newValue.size();
    for(int i=0; i<processCount; i++) {
        if(i>=m_rankingDepth)
            break;
        float readPercentage = 100.0;
        float writePercentage = 100.0;
        m_loggingFunction(QString("Read ranking: foo(%1%)").arg(readPercentage, 0, 'f', 1));
        m_loggingFunction(QString("Write ranking: foo(%1%)").arg(writePercentage, 0, 'f', 1));
    }
}
