#ifndef LOGCOMPONENT_H
#define LOGCOMPONENT_H

#include "abstractlogvaluegetter.h"
#include "logstrategyminmaxmean.h"

#include <QObject>
#include <QList>
#include <memory>

class LogComponent
{

public:
    LogComponent(std::unique_ptr<AbstractLogValueGetter> logValueGetter, std::unique_ptr<LogStrategyMinMaxMean> logStrategy);

    QList<float> getBuffer();
public slots:
    void tryLogOne();

private:
    std::unique_ptr<AbstractLogValueGetter> m_logValueGetter;
    std::unique_ptr<LogStrategyMinMaxMean> m_strategy;
    QList<float> m_buffer;
};

#endif // LOGCOMPONENT_H
