#include "logstrategyminmaxmean.h"

LogStrategyMinMaxMean::LogStrategyMinMaxMean(int valuesTillLog, QString valueLabel, QString unitLabel, std::function<void (QString)> loggingFunction) :
    m_logCollector(valuesTillLog),
    m_valueLabel(valueLabel),
    m_unitLabel(unitLabel),
    m_loggingFunction(loggingFunction)
{
}

void LogStrategyMinMaxMean::addValue(float newValue)
{
    QList<float> valuesForLog = m_logCollector.addValueAndReturnIfFull(newValue);
    if(valuesForLog.size()) {
        float min = *std::min_element(valuesForLog.begin(), valuesForLog.end());
        float max = *std::max_element(valuesForLog.begin(), valuesForLog.end());
        float mean = std::accumulate(valuesForLog.begin(), valuesForLog.end(), 0.0)/valuesForLog.size();
        QString logString = QString("%1 (%2) min: %3, max: %4, mean: %5").
                            arg(m_valueLabel, m_unitLabel).
                            arg(min, 0, 'f', 1).
                            arg(max, 0, 'f', 1).
                            arg(mean, 0, 'f', 1);
        m_loggingFunction(logString);
    }
}
