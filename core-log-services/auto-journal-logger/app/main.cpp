#include "logcreatorjournalsimple.h"
#include "cmdparser.h"
#include "cmdhandler.h"
#include <cmdserver.h>
#include <QCoreApplication>

int main( int argc, char *argv[] )
{
    QCoreApplication* app = new QCoreApplication(argc, argv);

    QSimpleCmdParserSocketBase::SetCmdLogGlobal(true);
    CmdHandler *handler = new CmdHandler(std::make_unique<LogCreatorJournalSimple>(), "/var/lib/zenux-coredumps/", app);
    CmdParser *parser = new CmdParser(app);
    parser->SetCmdHandler(handler);

    const int portNo = 5000;
    QSimpleCmdServer server;
    if(server.Setup(portNo, parser))
        qInfo("Opened logger server on port %u", portNo);
    else
        qWarning("Could not start I/O server on %u", portNo);
    return app->exec();
}
