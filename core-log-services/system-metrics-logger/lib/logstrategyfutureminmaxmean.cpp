#include "logstrategyfutureminmaxmean.h"

LogStrategyFutureMinMaxMean::LogStrategyFutureMinMaxMean(QString valueLabel, QString unitLabel) :
    m_valueLabel(valueLabel),
    m_unitLabel(unitLabel)
{
}

void LogStrategyFutureMinMaxMean::addValue(QList<float> &values, float newValue)
{
    if(!qIsNaN(newValue)) {
        values.append(newValue);
        if (values.size() >= 10) {
            float min = *std::min_element(values.begin(), values.end());
            float max = *std::max_element(values.begin(), values.end());
            float mean = std::accumulate(values.begin(), values.end(), 0.0)/values.size();
            qInfo("%s (%s) min: %.1f, max: %.1f, mean: %.1f",
                  qPrintable(m_valueLabel), qPrintable(m_unitLabel), min, max, mean);
            values.clear();
        }
    }
}
