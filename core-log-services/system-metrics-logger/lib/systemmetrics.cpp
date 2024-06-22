#include "systemmetrics.h"
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
