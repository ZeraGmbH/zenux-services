#ifndef LOGSTRATEGYFUTUREMINMAXMEAN_H
#define LOGSTRATEGYFUTUREMINMAXMEAN_H

#include "abstractlogstrategy.h"
#include <functional>

class LogStrategyFutureMinMaxMean : public AbstractLogStrategy
{
public:
    LogStrategyFutureMinMaxMean(QString valueLabel, QString unitLabel, std::function<void(QString)> loggingFunction = [](QString logString){
            qInfo("%s",qPrintable(logString));
        });
    void addValue(QList<float> &values, float newValue) override;
private:
    QString m_valueLabel;
    QString m_unitLabel;
    std::function<void(QString)> m_loggingFunction;
};

#endif // LOGSTRATEGYFUTUREMINMAXMEAN_H
