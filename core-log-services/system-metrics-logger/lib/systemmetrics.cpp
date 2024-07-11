#include "systemmetrics.h"
#include "cputemp.h"
#include "cpuload.h"
#include "cpufreq.h"
#include "logstrategyminmaxmean.h"

#include <timerfactoryqt.h>

TotalMemoryTracker *SystemMetrics::getTotalMemoryTracker()
{
    return &m_totalMemoryTracker;
}

void SystemMetrics::onMemoryTimer()
{
    m_totalMemoryTracker.calculateMemoryUsedPercent();
    m_totalMemoryTracker.outputLogs();
    emit sigNewMemValues();
}

void SystemMetrics::onLogComponentsTimer()
{
    for(auto &entry : m_logComponents)
        entry->tryLogOne();
}

void SystemMetrics::startMemoryPollTimer(int logIntervalMs)
{
    m_memoryPollTimer = TimerFactoryQt::createPeriodic(logIntervalMs);
    connect(m_memoryPollTimer.get(), &TimerTemplateQt::sigExpired,
            this, &SystemMetrics::onMemoryTimer);
    m_memoryPollTimer->start();
    onMemoryTimer();
}

void SystemMetrics::startLogComponentsTimer(int pollMs)
{
    m_logComponentsTimer = TimerFactoryQt::createPeriodic(pollMs);
    connect(m_logComponentsTimer.get(), &TimerTemplateQt::sigExpired,
            this, &SystemMetrics::onLogComponentsTimer);
    m_logComponentsTimer->start();
    onLogComponentsTimer();
}

void SystemMetrics::initLogComponents()
{
    std::unique_ptr<AbstractLogValueGetter> currValueGetter;

    currValueGetter = std::make_unique<CpuTemp>();
    if(currValueGetter->canGetValue())
        m_logComponents.push_back(std::make_unique<LogComponent>(std::make_unique<CpuTemp>(), std::make_unique<LogStrategyMinMaxMean>("CPU Temperature")));
    else
        qWarning("CpuTemp does not work in this environment - ignore!");

    currValueGetter = std::make_unique<CpuLoad>(0);
    if(currValueGetter->canGetValue())
        m_logComponents.push_back(std::make_unique<LogComponent>(std::make_unique<CpuLoad>(0), std::make_unique<LogStrategyMinMaxMean>("CPU Load")));
    else
        qWarning("CpuLoad does not work in this environment - ignore");

    currValueGetter = std::make_unique<CpuFreq>();
    if(currValueGetter->canGetValue())
        m_logComponents.push_back(std::make_unique<LogComponent>(std::make_unique<CpuFreq>(), std::make_unique<LogStrategyMinMaxMean>("CPU Frequency")));
    else
        qWarning("CpuFrequency does not work in this environment - ignore");
}
