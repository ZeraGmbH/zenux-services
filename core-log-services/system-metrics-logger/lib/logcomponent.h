#ifndef LOGCOMPONENT_H
#define LOGCOMPONENT_H

#include "logvaluegetter.h"
#include "logstrategy.h"

#include <QObject>
#include <QList>
#include <memory>

class LogComponent
{

public:
    LogComponent(std::unique_ptr<LogValueGetter> logValueGetter, std::unique_ptr<LogStrategy> logStrategy);

    QList<float> getBuffer();
public slots:
    void tryLogOne();

private:
    std::unique_ptr<LogValueGetter> m_logValueGetter;
    std::unique_ptr<LogStrategy> m_strategy;
    QList<float> m_buffer;
};

#endif // LOGCOMPONENT_H
