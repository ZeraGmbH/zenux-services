#ifndef LOGSTRATEGYMINMAXMEAN_H
#define LOGSTRATEGYMINMAXMEAN_H

#include "abstractlogstrategy.h"
#include <functional>

class LogStrategyMinMaxMean : public AbstractLogStrategy
{
public:
    LogStrategyMinMaxMean(int valuesTillLog, QString valueLabel, QString unitLabel, std::function<void(QString)> loggingFunction = [](QString logString){
            qInfo("%s",qPrintable(logString));
        });
    void addValue(QList<float> &values, float newValue) override;
private:
    int m_valuesTillLog;
    QString m_valueLabel;
    QString m_unitLabel;
    std::function<void(QString)> m_loggingFunction;
};

#endif // LOGSTRATEGYMINMAXMEAN_H
