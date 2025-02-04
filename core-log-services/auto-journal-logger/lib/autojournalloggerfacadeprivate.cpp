#include "autojournalloggerfacadeprivate.h"

AutoJournalLoggerFacadePrivate::AutoJournalLoggerFacadePrivate(QByteArray codedumpDir,
                                                               int portNo,
                                                               AbstractLogCreatorPtr logGenerator) :
    m_cmdHandler(std::move(logGenerator), codedumpDir)
{
    QSimpleCmdParserSocketBase::SetCmdLogGlobal(true);
    m_cmdParser.SetCmdHandler(&m_cmdHandler);
    if(m_server.Setup(portNo, &m_cmdParser))
        qInfo("Opened logger server on port %u", portNo);
    else
        qWarning("Could not start I/O server on %u", portNo);
}
