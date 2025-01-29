#ifndef AUTOJOURNALLOGGERFACADEPRIVATE_H
#define AUTOJOURNALLOGGERFACADEPRIVATE_H

#include "cmdparser.h"
#include "cmdhandler.h"
#include <cmdserver.h>

// PIMPL: Avoid linking simple-parser public
class AutoJournalLoggerFacadePrivate
{
public:
    AutoJournalLoggerFacadePrivate(QByteArray codedumpDir, int portNo, AbstractLogCreatorPtr logGenerator);
    CmdHandler m_cmdHandler;
    CmdParser m_cmdParser;
    QSimpleCmdServer m_server;
};

#endif // AUTOJOURNALLOGGERFACADEPRIVATE_H
