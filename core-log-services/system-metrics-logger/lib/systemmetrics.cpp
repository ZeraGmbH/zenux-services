#include "systemmetrics.h"
#include "cputemp.h"
#include "cpuload.h"
#include "cpufreq.h"
#include "diskreadtotal.h"
#include "diskwritetotal.h"
#include "logstrategyminmaxmean.h"
#include "totalmemorytracker.h"
#include "fpgainterrupts.h"
#include "procdiskstatdecoder.h"
#include <timerfactoryqt.h>
#include <QDebug>

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
    std::unique_ptr<AbstractLogValueGetter<float>> currValueFloatGetter;

    currValueFloatGetter = std::make_unique<CpuTemp>();
    if(currValueFloatGetter->canGetValue())
        m_logComponents.push_back(std::make_unique<LogComponent<float>>(std::move(currValueFloatGetter), std::make_unique<LogStrategyMinMaxMean>(10, "CPU Temperature", "°C")));
    else
        qWarning("CpuTemp does not work in this environment - ignore!");

    currValueFloatGetter = std::make_unique<CpuLoad>(0);
    if(currValueFloatGetter->canGetValue())
        m_logComponents.push_back(std::make_unique<LogComponent<float>>(std::move(currValueFloatGetter), std::make_unique<LogStrategyMinMaxMean>(10, "CPU Load", "%")));
    else
        qWarning("CpuLoad does not work in this environment - ignore");

    currValueFloatGetter = std::make_unique<CpuFreq>();
    if(currValueFloatGetter->canGetValue())
        m_logComponents.push_back(std::make_unique<LogComponent<float>>(std::move(currValueFloatGetter), std::make_unique<LogStrategyMinMaxMean>(10, "CPU Frequency", "MHz")));
    else
        qWarning("CpuFrequency does not work in this environment - ignore");

    currValueFloatGetter = std::make_unique<TotalMemoryTracker>();
    if(currValueFloatGetter->canGetValue())
        m_logComponents.push_back(std::make_unique<LogComponent<float>>(std::move(currValueFloatGetter), std::make_unique<LogStrategyMinMaxMean>(10, "RAM usage", "%")));
    else
        qWarning("RAM usage does not work in this environment - ignore");

    currValueFloatGetter = std::make_unique<FpgaInterrupts>();
    if(currValueFloatGetter->canGetValue())
        m_logComponents.push_back(std::make_unique<LogComponent<float>>(std::move(currValueFloatGetter), std::make_unique<LogStrategyMinMaxMean>(10, "Fpga Interrupts", "interrupt/s")));
    else
        qWarning("FPGA interrupts do not work in this environment - ignore");

    QStringList diskDevices = ProcDiskStatDecoder::getDiskBlockDevicesOfInterest();
    qInfo() << "Drives to monitor:" << diskDevices;
    currValueFloatGetter = std::make_unique<DiskReadTotal>(std::make_unique<DiskIoTotalCalculator>(diskDevices));
    if(currValueFloatGetter->canGetValue())
        m_logComponents.push_back(std::make_unique<LogComponent<float>>(std::move(currValueFloatGetter), std::make_unique<LogStrategyMinMaxMean>(60, "Drive reads", "KiB/s")));
    else
        qWarning("Drive read monitoring does work in this environment - ignore");
    currValueFloatGetter = std::make_unique<DiskWriteTotal>(std::make_unique<DiskIoTotalCalculator>(diskDevices));
    if(currValueFloatGetter->canGetValue())
        m_logComponents.push_back(std::make_unique<LogComponent<float>>(std::move(currValueFloatGetter), std::make_unique<LogStrategyMinMaxMean>(60, "Drive writes", "KiB/s")));
    else
        qWarning("Drive write monitoring does work in this environment - ignore");
}
