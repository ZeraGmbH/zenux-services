#ifndef LOGCOMPONENT_H
#define LOGCOMPONENT_H

#include "abstractlogvaluegetter.h"
#include "abstractlogstrategy.h"
#include <QList>
#include <memory>

class LogComponent
{
public:
    LogComponent(std::unique_ptr<AbstractLogValueGetter> logValueGetter, std::unique_ptr<AbstractLogStrategy> logStrategy);
    QList<float> getBuffer();
    void tryLogOne();

private:
    std::unique_ptr<AbstractLogValueGetter> m_logValueGetter;
    std::unique_ptr<AbstractLogStrategy> m_strategy;
    QList<float> m_buffer;
};

#endif // LOGCOMPONENT_H
