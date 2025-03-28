#ifndef LOGSTRATEGYMINMAXMEAN_H
#define LOGSTRATEGYMINMAXMEAN_H

#include "abstractlogstrategy.h"
#include "logcollectorfloatingnumbers.h"
#include <functional>

class LogStrategyMinMaxMean : public AbstractLogStrategy<float>
{
public:
    LogStrategyMinMaxMean(int valuesTillLog, QString valueLabel, QString unitLabel, std::function<void(QString)> loggingFunction);
    void addValue(float newValue) override;
private:
    LogCollectorFloatingNumbers<float> m_logCollector;
    QString m_valueLabel;
    QString m_unitLabel;
    std::function<void(QString)> m_loggingFunction;
};

#endif // LOGSTRATEGYMINMAXMEAN_H
