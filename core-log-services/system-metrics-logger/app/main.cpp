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
#include <QCoreApplication>
#include <QDebug>
#include <memory>

static void init10SecComponents(std::vector<std::unique_ptr<LogComponent>> &logComponents)
{
    std::unique_ptr<AbstractLogValueGetter> currValueGetter;

    currValueGetter = std::make_unique<CpuTemp>();
    if(currValueGetter->canGetValue())
        logComponents.push_back(std::make_unique<LogComponent>(std::move(currValueGetter), std::make_unique<LogStrategyMinMaxMean>("CPU Temperature", "°C")));
    else
        qWarning("CpuTemp does not work in this environment - ignore!");

    currValueGetter = std::make_unique<CpuLoad>(0);
    if(currValueGetter->canGetValue())
        logComponents.push_back(std::make_unique<LogComponent>(std::move(currValueGetter), std::make_unique<LogStrategyMinMaxMean>("CPU Load", "%")));
    else
        qWarning("CpuLoad does not work in this environment - ignore");

    currValueGetter = std::make_unique<CpuFreq>();
    if(currValueGetter->canGetValue())
        logComponents.push_back(std::make_unique<LogComponent>(std::move(currValueGetter), std::make_unique<LogStrategyMinMaxMean>("CPU Frequency", "MHz")));
    else
        qWarning("CpuFrequency does not work in this environment - ignore");

    currValueGetter = std::make_unique<TotalMemoryTracker>();
    if(currValueGetter->canGetValue())
        logComponents.push_back(std::make_unique<LogComponent>(std::move(currValueGetter), std::make_unique<LogStrategyMinMaxMean>("RAM usage", "%")));
    else
        qWarning("RAM usage does not work in this environment - ignore");

    currValueGetter = std::make_unique<FpgaInterrupts>();
    if(currValueGetter->canGetValue())
        logComponents.push_back(std::make_unique<LogComponent>(std::move(currValueGetter), std::make_unique<LogStrategyMinMaxMean>("Fpga Interrupts", "interrupt/s")));
    else
        qWarning("FPGA interrupts do not work in this environment - ignore");

}

static void init60SecComponents(std::vector<std::unique_ptr<LogComponent>> &logComponents)
{
    std::unique_ptr<AbstractLogValueGetter> currValueGetter;

    QStringList diskDevices = ProcDiskStatDecoder::getDiskBlockDevicesOfInterest();
    qInfo() << "Drives to monitor:" << diskDevices;
    currValueGetter = std::make_unique<DiskReadTotal>(std::make_unique<DiskIoTotalCalculator>(diskDevices));
    if(currValueGetter->canGetValue())
        logComponents.push_back(std::make_unique<LogComponent>(std::move(currValueGetter), std::make_unique<LogStrategyMinMaxMean>("Drive reads", "KiB/s")));
    else
        qWarning("Drive read monitoring does work in this environment - ignore");
    currValueGetter = std::make_unique<DiskWriteTotal>(std::make_unique<DiskIoTotalCalculator>(diskDevices));
    if(currValueGetter->canGetValue())
        logComponents.push_back(std::make_unique<LogComponent>(std::move(currValueGetter), std::make_unique<LogStrategyMinMaxMean>("Drive writes", "KiB/s")));
    else
        qWarning("Drive write monitoring does work in this environment - ignore");
}


int main( int argc, char *argv[] )
{
    std::unique_ptr<QCoreApplication> app = std::make_unique<QCoreApplication>(argc, argv);

    std::vector<std::unique_ptr<LogComponent>> logComponents;

    std::unique_ptr<SystemMetrics> systemMetrics10s = std::make_unique<SystemMetrics>();
    init10SecComponents(logComponents);
    systemMetrics10s->initLogComponents(logComponents);

    std::unique_ptr<SystemMetrics> systemMetrics60s = std::make_unique<SystemMetrics>();
    init60SecComponents(logComponents);
    systemMetrics60s->initLogComponents(logComponents);

    systemMetrics10s->startLogComponentsTimer(1000);
    systemMetrics60s->startLogComponentsTimer(1000);

    return app->exec();
}
