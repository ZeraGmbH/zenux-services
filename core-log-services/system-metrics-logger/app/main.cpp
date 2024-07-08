#include "systemmetrics.h"
#include <QCoreApplication>
#include <memory>

int main( int argc, char *argv[] )
{
    std::unique_ptr<QCoreApplication> app = std::make_unique<QCoreApplication>(argc, argv);

    std::unique_ptr<SystemMetrics> systemMetrics = std::make_unique<SystemMetrics>();
    systemMetrics->initLogComponents();
    systemMetrics->startLogComponentsTimer(1000);
    systemMetrics->startCpuLoadPollTimer(1000);
    systemMetrics->startMemoryPollTimer(10000);
    systemMetrics->getCpuLoad()->setWarningLimit(0.2);

    return app->exec();
}
