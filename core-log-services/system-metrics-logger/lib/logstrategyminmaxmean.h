#ifndef LOGSTRATEGYMINMAXMEAN_H
#define LOGSTRATEGYMINMAXMEAN_H

#include "abstractlogstrategy.h"

class LogStrategyMinMaxMean : public AbstractLogStrategy
{
public:
    LogStrategyMinMaxMean(QString valueLabel);
    void addValue(QList<float> &values, float newValue) override;
private:
    QString m_valueLabel;
};

#endif // LOGSTRATEGYMINMAXMEAN_H
