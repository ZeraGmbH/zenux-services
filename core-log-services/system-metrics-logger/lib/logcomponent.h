#ifndef LOGCOMPONENT_H
#define LOGCOMPONENT_H

#include "abstractlogvaluegetter.h"
#include "abstractlogstrategy.h"
#include <QList>
#include <memory>

template <class T>
class LogComponent
{
public:
    LogComponent(std::unique_ptr<AbstractLogValueGetter<T>> logValueGetter, std::unique_ptr<AbstractLogStrategy<T>> logStrategy) :
        m_logValueGetter(std::move(logValueGetter)),
        m_strategy(std::move(logStrategy)) {
        // Most getters calculate diffs from last read. For them to have a proper
        // last value read once and ignore
        m_logValueGetter->getValue();
    }
    void tryLogOne() {
        m_strategy->addValue(m_logValueGetter->getValue());
    }

private:
    std::unique_ptr<AbstractLogValueGetter<T>> m_logValueGetter;
    std::unique_ptr<AbstractLogStrategy<T>> m_strategy;
};

#endif // LOGCOMPONENT_H
