#include "logcomponent.h"

LogComponent::LogComponent(std::unique_ptr<AbstractLogValueGetter> logValueGetter, std::unique_ptr<AbstractLogStrategy> logStrategy) :
    m_logValueGetter(std::move(logValueGetter)),
    m_strategy(std::move(logStrategy))
{
    // Most getters calculate diffs from last read. For them to have a proper
    // last value read once and ignore
    m_logValueGetter->getValue();
}

QList<float> LogComponent::getBuffer()
{
    return m_buffer;
}

void LogComponent::tryLogOne()
{
    m_strategy->addValue(m_buffer, m_logValueGetter->getValue());
}
