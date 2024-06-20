#include "logcreatorjournalsimple.h"
#include "cmdparser.h"
#include "cmdhandler.h"
#include <cmdserver.h>
#include <QCoreApplication>

int main( int argc, char *argv[] )
{
    QCoreApplication* app = new QCoreApplication(argc, argv);

    CmdHandler *handler = new CmdHandler(std::make_unique<LogCreatorJournalSimple>(), "/var/lib/systemd/coredump/", app);
    CmdParser *parser = new CmdParser(app);
    parser->SetCmdHandler(handler);

    QSimpleCmdParserSocketBase::SetCmdLogGlobal(true); // ??

    const int portNo = 5000;
    QSimpleCmdServer server;
    if(server.Setup(portNo, parser))
        qInfo("Opened logger server on port %u", portNo);
    else
        qWarning("Could not start I/O server on %u", portNo);
    return app->exec();
}
