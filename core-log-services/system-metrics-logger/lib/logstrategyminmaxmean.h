#ifndef LOGSTRATEGYMINMAXMEAN_H
#define LOGSTRATEGYMINMAXMEAN_H

#include "abstractlogstrategy.h"

class LogStrategyMinMaxMean : public AbstractLogStrategy
{
public:
    void addValue(QList<float> &values, float newValue) override;
};

#endif // LOGSTRATEGYMINMAXMEAN_H
