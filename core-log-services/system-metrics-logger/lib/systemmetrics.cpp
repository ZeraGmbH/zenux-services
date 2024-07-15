#include "systemmetrics.h"
#include "cputemp.h"
#include "cpuload.h"
#include "cpufreq.h"
#include "logstrategyminmaxmean.h"
#include "logstrategyfutureminmaxmean.h"
#include "totalmemorytracker.h"
#include "fpgainterrupts.h"

#include <timerfactoryqt.h>

void SystemMetrics::onLogComponentsTimer()
{
    for(auto &entry : m_logComponents)
        entry->tryLogOne();
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
        m_logComponents.push_back(std::make_unique<LogComponent>(std::make_unique<CpuTemp>(), std::make_unique<LogStrategyMinMaxMean>("CPU Temperature", "°C")));
    else
        qWarning("CpuTemp does not work in this environment - ignore!");

    currValueGetter = std::make_unique<CpuLoad>(0);
    if(currValueGetter->canGetValue())
        m_logComponents.push_back(std::make_unique<LogComponent>(std::make_unique<CpuLoad>(0), std::make_unique<LogStrategyMinMaxMean>("CPU Load", "%")));
    else
        qWarning("CpuLoad does not work in this environment - ignore");

    currValueGetter = std::make_unique<CpuFreq>();
    if(currValueGetter->canGetValue())
        m_logComponents.push_back(std::make_unique<LogComponent>(std::make_unique<CpuFreq>(), std::make_unique<LogStrategyMinMaxMean>("CPU Frequency", "MHz")));
    else
        qWarning("CpuFrequency does not work in this environment - ignore");

    currValueGetter = std::make_unique<TotalMemoryTracker>();
    if(currValueGetter->canGetValue())
        m_logComponents.push_back(std::make_unique<LogComponent>(std::make_unique<TotalMemoryTracker>(), std::make_unique<LogStrategyMinMaxMean>("RAM usage", "%")));
    else
        qWarning("RAM usage does not work in this environment - ignore");

    currValueGetter = std::make_unique<FpgaInterrupts>();
    if(currValueGetter->canGetValue())
        m_logComponents.push_back(std::make_unique<LogComponent>(std::make_unique<FpgaInterrupts>(), std::make_unique<LogStrategyFutureMinMaxMean>("Fpga Interrupts", "interrupt/s")));
    else
        qWarning("FPGA interrupts do not work in this environment - ignore");
}
