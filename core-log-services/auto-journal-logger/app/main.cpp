#include "autojournalloggerfacade.h"
#include <QCoreApplication>
#include <memory>

int main( int argc, char *argv[] )
{
    std::unique_ptr<QCoreApplication> app = std::make_unique<QCoreApplication>(argc, argv);
    AutoJournalLoggerFacade autoLogger;
    return app->exec();
}
