#include "logstrategyminmaxmean.h"

LogStrategyMinMaxMean::LogStrategyMinMaxMean() {}

void LogStrategyMinMaxMean::addValue(QList<float> &values, float newValue)
{
    values.append(newValue);
    if (values.size() >= 10) {
        float min = *std::min_element(values.begin(), values.end());
        float max = *std::max_element(values.begin(), values.end());
        float mean = std::accumulate(values.begin(), values.end(), 0.0)/values.size();
        qInfo("CPU Temperature min: %.1f, max: %.1f, mean: %.1f", min, max, mean);
        values.clear();
    }
}
