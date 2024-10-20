#include "systemmetrics.h"
#include <QCoreApplication>
#include <memory>

int main( int argc, char *argv[] )
{
    std::unique_ptr<QCoreApplication> app = std::make_unique<QCoreApplication>(argc, argv);

    std::unique_ptr<SystemMetrics> systemMetrics = std::make_unique<SystemMetrics>();
    systemMetrics->initLogComponents();
    systemMetrics->startLogComponentsTimer();

    return app->exec();
}
