#ifndef LOGSTRATEGYMINMAXMEAN_H
#define LOGSTRATEGYMINMAXMEAN_H

#include "abstractlogstrategy.h"

class LogStrategyMinMaxMean : public AbstractLogStrategy
{
public:
    LogStrategyMinMaxMean(QString valueLabel, QString unitLabel);
    void addValue(QList<float> &values, float newValue) override;
private:
    QString m_valueLabel;
    QString m_unitLabel;
};

#endif // LOGSTRATEGYMINMAXMEAN_H
