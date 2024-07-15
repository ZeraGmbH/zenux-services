#ifndef LOGSTRATEGYFUTUREMINMAXMEAN_H
#define LOGSTRATEGYFUTUREMINMAXMEAN_H

#include "abstractlogstrategy.h"

class LogStrategyFutureMinMaxMean : public AbstractLogStrategy
{
public:
    LogStrategyFutureMinMaxMean(QString valueLabel, QString unitLabel);
    void addValue(QList<float> &values, float newValue) override;
private:
    QString m_valueLabel;
    QString m_unitLabel;
};

#endif // LOGSTRATEGYFUTUREMINMAXMEAN_H
