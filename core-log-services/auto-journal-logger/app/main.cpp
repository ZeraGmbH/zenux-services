#include "autojournalloggerfacade.h"
#include "logcreatorjournalsimple.h"
#include <QCoreApplication>
#include <memory>

int main( int argc, char *argv[] )
{
    std::unique_ptr<QCoreApplication> app = std::make_unique<QCoreApplication>(argc, argv);
    AutoJournalLoggerFacade autoLogger(std::make_unique<LogCreatorJournalSimple>());
    return app->exec();
}
