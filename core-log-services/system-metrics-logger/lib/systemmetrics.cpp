#include "systemmetrics.h"
#include "cputemp.h"
#include "cpuload.h"
#include "cpufreq.h"
#include "diskioforallprocesses.h"
#include "diskreadtotal.h"
#include "diskwritetotal.h"
#include "logstrategyminmaxmean.h"
#include "processcount.h"
#include "totalmemorytracker.h"
#include "fpgainterrupts.h"
#include "procdiskstatdecoder.h"
#include "logstrategydiskiotopranking.h"
#include <timerfactoryqt.h>
#include <QDebug>

SystemMetrics::SystemMetrics(std::function<void (QString)> loggingFunction) :
    m_loggingFunction(loggingFunction)
{
}

void SystemMetrics::onLogComponentsTimer()
{
    for(const auto &entry : m_logComponents)
        entry->tryLogOne();
    m_60sCounter++;
    if(m_60sCounter >= 60) {
        m_60sCounter = 0;
        for(const auto &entry : m_logComponents60s)
            entry->tryLogOne();
    }
}

void SystemMetrics::initLogComponents()
{
    init1sClocked10sResultLoggers();
    init1sClocked60sResultLoggers();
    init60sClockedOneShotResultLoggers();
}

void SystemMetrics::startLogComponentsTimer()
{
    m_1sPeriodicTimer = TimerFactoryQt::createPeriodic(1000);
    connect(m_1sPeriodicTimer.get(), &TimerTemplateQt::sigExpired,
            this, &SystemMetrics::onLogComponentsTimer);
    m_1sPeriodicTimer->start();
    onLogComponentsTimer();
}

void SystemMetrics::init1sClocked10sResultLoggers()
{
    std::unique_ptr<AbstractLogValueGetter<float>> currValueFloatGetter;

    currValueFloatGetter = std::make_unique<CpuTemp>();
    if(currValueFloatGetter->canGetValue())
        m_logComponents.push_back(std::make_unique<LogComponent<float>>(std::move(currValueFloatGetter), std::make_unique<LogStrategyMinMaxMean>(10, "CPU Temperature", "°C", m_loggingFunction)));
    else
        qWarning("CpuTemp does not work in this environment - ignore!");

    currValueFloatGetter = std::make_unique<CpuLoad>(0);
    if(currValueFloatGetter->canGetValue())
        m_logComponents.push_back(std::make_unique<LogComponent<float>>(std::move(currValueFloatGetter), std::make_unique<LogStrategyMinMaxMean>(10, "CPU Load", "%", m_loggingFunction)));
    else
        qWarning("CpuLoad does not work in this environment - ignore");

    currValueFloatGetter = std::make_unique<CpuFreq>();
    if(currValueFloatGetter->canGetValue())
        m_logComponents.push_back(std::make_unique<LogComponent<float>>(std::move(currValueFloatGetter), std::make_unique<LogStrategyMinMaxMean>(10, "CPU Frequency", "MHz", m_loggingFunction)));
    else
        qWarning("CpuFrequency does not work in this environment - ignore");

    currValueFloatGetter = std::make_unique<TotalMemoryTracker>();
    if(currValueFloatGetter->canGetValue())
        m_logComponents.push_back(std::make_unique<LogComponent<float>>(std::move(currValueFloatGetter), std::make_unique<LogStrategyMinMaxMean>(10, "RAM usage", "%", m_loggingFunction)));
    else
        qWarning("RAM usage does not work in this environment - ignore");

    currValueFloatGetter = std::make_unique<FpgaInterrupts>();
    if(currValueFloatGetter->canGetValue())
        m_logComponents.push_back(std::make_unique<LogComponent<float>>(std::move(currValueFloatGetter), std::make_unique<LogStrategyMinMaxMean>(10, "Fpga Interrupts", "interrupt/s", m_loggingFunction)));
    else
        qWarning("FPGA interrupts do not work in this environment - ignore");
    currValueFloatGetter = std::make_unique<ProcessCount>();
    if(currValueFloatGetter->canGetValue())
        m_logComponents.push_back(std::make_unique<LogComponent<float>>(std::move(currValueFloatGetter), std::make_unique<LogStrategyMinMaxMean>(10, "Process count", "", m_loggingFunction)));
    else
        qWarning("ProcessCount does not work in this environment - ignore!");
}

void SystemMetrics::init1sClocked60sResultLoggers()
{
    std::unique_ptr<AbstractLogValueGetter<float>> currValueFloatGetter;

    QStringList diskDevices = ProcDiskStatDecoder::getDiskBlockDevicesOfInterest();
    qInfo() << "Drives to monitor:" << diskDevices;
    currValueFloatGetter = std::make_unique<DiskReadTotal>(std::make_unique<DiskIoTotalCalculator>(diskDevices));
    if(currValueFloatGetter->canGetValue())
        m_logComponents.push_back(std::make_unique<LogComponent<float>>(std::move(currValueFloatGetter), std::make_unique<LogStrategyMinMaxMean>(60, "Drive reads", "KiB/s", m_loggingFunction)));
    else
        qWarning("Drive read monitoring does work in this environment - ignore");
    currValueFloatGetter = std::make_unique<DiskWriteTotal>(std::make_unique<DiskIoTotalCalculator>(diskDevices));
    if(currValueFloatGetter->canGetValue())
        m_logComponents.push_back(std::make_unique<LogComponent<float>>(std::move(currValueFloatGetter), std::make_unique<LogStrategyMinMaxMean>(60, "Drive writes", "KiB/s", m_loggingFunction)));
    else
        qWarning("Drive write monitoring does work in this environment - ignore");
}

void SystemMetrics::init60sClockedOneShotResultLoggers()
{
    std::unique_ptr<AbstractLogValueGetter<DiskValuesProcesses>> currValueGetter;
    currValueGetter = std::make_unique<DiskIoForAllProcesses>();
    if(currValueGetter->canGetValue())
        m_logComponents60s.push_back(std::make_unique<LogComponent<DiskValuesProcesses>>(std::move(currValueGetter), std::make_unique<LogStrategyDiskIoTopRanking>(5, 60*1000, m_loggingFunction)));
    else
        qWarning("Read / Write ranking does not work here (missing root privileges?) - ignore");
}
