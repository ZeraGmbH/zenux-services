#include "systemmetrics.h"
#include "cputemp.h"
#include "logstrategyminmaxmean.h"

#include <timerfactoryqt.h>

CpuLoad *SystemMetrics::getCpuLoad()
{
    return &m_cpuLoad;
}

TotalMemoryTracker *SystemMetrics::getTotalMemoryTracker()
{
    return &m_totalMemoryTracker;
}

void SystemMetrics::onCpuLoadTimer()
{
    m_cpuLoad.calcNextValues();
    emit sigNewCpuValues();
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

void SystemMetrics::startCpuLoadPollTimer(int pollMs)
{
    m_cpuLoadPollTimer = TimerFactoryQt::createPeriodic(pollMs);
    connect(m_cpuLoadPollTimer.get(), &TimerTemplateQt::sigExpired,
            this, &SystemMetrics::onCpuLoadTimer);
    m_cpuLoadPollTimer->start();
    onCpuLoadTimer();
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
        m_logComponents.push_back(std::make_unique<LogComponent>(std::make_unique<CpuTemp>(), std::make_unique<LogStrategyMinMaxMean>()));
    else
        qWarning("CpuTemp does not work in this environment - ignore!");

    // more to come...
}
