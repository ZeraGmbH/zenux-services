#ifndef AUTOJOURNALLOGGERFACADEPRIVATE_H
#define AUTOJOURNALLOGGERFACADEPRIVATE_H

#include "cmdparser.h"
#include "cmdhandler.h"
#include <cmdserver.h>

class AutoJournalLoggerFacadePrivate
{
public:
    AutoJournalLoggerFacadePrivate(QByteArray codedumpDir, int portNo);
    CmdHandler m_cmdHandler;
    CmdParser m_cmdParser;
    QSimpleCmdServer m_server;
};

#endif // AUTOJOURNALLOGGERFACADEPRIVATE_H
