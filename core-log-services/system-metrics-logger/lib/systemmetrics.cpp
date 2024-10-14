#include "systemmetrics.h"
#include <timerfactoryqt.h>
#include <QDebug>

void SystemMetrics::onLogComponentsTimer()
{
    for(auto &entry : m_logComponents)
        entry->tryLogOne();
}

void SystemMetrics::initLogComponents(std::vector<std::unique_ptr<LogComponent> > &logComponents)
{
    for(auto &component : logComponents)
        m_logComponents.push_back(std::move(component));
    logComponents.clear();
}

void SystemMetrics::startLogComponentsTimer(int pollMs)
{
    m_logComponentsTimer = TimerFactoryQt::createPeriodic(pollMs);
    connect(m_logComponentsTimer.get(), &TimerTemplateQt::sigExpired,
            this, &SystemMetrics::onLogComponentsTimer);
    m_logComponentsTimer->start();
    onLogComponentsTimer();
}

