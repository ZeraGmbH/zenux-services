#include "cmdparser.h"
#include "cmdhandler.h"
#include "cmdserver.h"
#include "periodicloggerwrapper.h"
#include <QCoreApplication>

int main( int argc, char *argv[] )
{
    QCoreApplication* app = new QCoreApplication(argc, argv);

    PeriodicLoggerWrapper *logger = new PeriodicLoggerWrapper(app);
    CmdHandler *handler = new CmdHandler(logger, app);
    CmdParser *parser = new CmdParser(logger, app);
    parser->SetCmdHandler(handler);

    QSimpleCmdParserSocketBase::SetCmdLogGlobal(true); // ??

    const int portNo = 5000;
    CmdServer server;
    if(server.Setup(portNo, parser))
        qInfo("Opened logger server on port %u", portNo);
    else
        qWarning("Could not start I/O server on %u", portNo);
    return app->exec();
}
