#ifndef LOGSTRATEGYMINMAXMEAN_H
#define LOGSTRATEGYMINMAXMEAN_H

#include <QList>

class LogStrategyMinMaxMean
{
public:
    void addValue(QList<float> &values, float newValue);
};

#endif // LOGSTRATEGYMINMAXMEAN_H
