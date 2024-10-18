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
    LogComponent(std::unique_ptr<AbstractLogValueGetter<T> > logValueGetter, std::unique_ptr<AbstractLogStrategy> logStrategy) :
        m_logValueGetter(std::move(logValueGetter)),
        m_strategy(std::move(logStrategy)) {
        // Most getters calculate diffs from last read. For them to have a proper
        // last value read once and ignore
        m_logValueGetter->getValue();
    }
    QList<T> getBuffer() {
        return m_buffer;
    }
    void tryLogOne() {
        m_strategy->addValue(m_buffer, m_logValueGetter->getValue());
    }

private:
    std::unique_ptr<AbstractLogValueGetter<T>> m_logValueGetter;
    std::unique_ptr<AbstractLogStrategy> m_strategy;
    QList<T> m_buffer;
};

#endif // LOGCOMPONENT_H
