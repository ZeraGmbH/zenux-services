#ifndef LOGSTRATEGYMINMAXMEAN_H
#define LOGSTRATEGYMINMAXMEAN_H

#include "abstractlogstrategy.h"
#include <functional>

class LogStrategyMinMaxMean : public AbstractLogStrategy
{
public:
    LogStrategyMinMaxMean(QString valueLabel, QString unitLabel, std::function<void(QString)> loggingFunction = [](QString logString){
            qInfo("%s",qPrintable(logString));
        });
    void addValue(QList<float> &values, float newValue) override;
private:
    QString m_valueLabel;
    QString m_unitLabel;
    std::function<void(QString)> m_loggingFunction;
};

#endif // LOGSTRATEGYMINMAXMEAN_H
