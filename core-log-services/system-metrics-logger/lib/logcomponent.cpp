#include "logcomponent.h"

LogComponent::LogComponent(std::unique_ptr<LogValueGetter> logValueGetter, std::unique_ptr<LogStrategy> logStrategy) :
    m_logValueGetter(std::move(logValueGetter)),
    m_strategy(std::move(logStrategy))
{}

QList<float> LogComponent::getBuffer()
{
    return m_buffer;
}

void LogComponent::tryLogOne()
{
    m_strategy->addValue(m_buffer, m_logValueGetter->getValue());
}
