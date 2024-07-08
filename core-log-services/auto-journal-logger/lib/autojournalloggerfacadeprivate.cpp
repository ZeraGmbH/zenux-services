#include "autojournalloggerfacadeprivate.h"
#include "logcreatorjournalsimple.h"

AutoJournalLoggerFacadePrivate::AutoJournalLoggerFacadePrivate(QByteArray codedumpDir, int portNo) :
    m_cmdHandler(std::make_unique<LogCreatorJournalSimple>(), codedumpDir)
{
    QSimpleCmdParserSocketBase::SetCmdLogGlobal(true);
    m_cmdParser.SetCmdHandler(&m_cmdHandler);
    if(m_server.Setup(portNo, &m_cmdParser))
        qInfo("Opened logger server on port %u", portNo);
    else
        qWarning("Could not start I/O server on %u", portNo);
}
