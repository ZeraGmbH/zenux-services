#include "logstrategyfutureminmaxmean.h"

LogStrategyFutureMinMaxMean::LogStrategyFutureMinMaxMean(QString valueLabel, QString unitLabel, std::function<void (QString)> loggingFunction) :
    m_valueLabel(valueLabel),
    m_unitLabel(unitLabel),
    m_loggingFunction(loggingFunction)
{
}

void LogStrategyFutureMinMaxMean::addValue(QList<float> &values, float newValue)
{
    values.append(newValue);
    if (values.size() >= 10) {
        QList<float> noNaNValues;
        for(auto value : values) {
            if(!qIsNaN(value))
                noNaNValues.append(value);
        }
        if(!noNaNValues.isEmpty()) {
            float min = *std::min_element(noNaNValues.begin(), noNaNValues.end());
            float max = *std::max_element(noNaNValues.begin(), noNaNValues.end());
            float mean = std::accumulate(noNaNValues.begin(), noNaNValues.end(), 0.0)/noNaNValues.size();
            QString logString = QString("%1 (%2) min: %3, max: %4, mean: %5").
                                arg(m_valueLabel, m_unitLabel).
                                arg(min, 0, 'f', 1).
                                arg(max, 0, 'f', 1).
                                arg(mean, 0, 'f', 1);
            m_loggingFunction(logString);
        }
        values.clear();
    }
}
