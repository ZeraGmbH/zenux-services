#include "scpicallbackmock.h"

ScpiCallbackMock::ScpiCallbackMock(cParseZdsp* parser) :
    m_parser(parser)
{
}

QString ScpiCallbackMock::SCPICmd(SCPICmdType cmdEnum, QChar *scpi)
{
    TCommand cmd;
    cmd.type = cmdEnum;
    const char *test = (const char *)scpi;
    cmd.cmd = QString::fromLatin1(test, -1);
    m_commandsReceived.append(cmd);
    return "";
}

QString ScpiCallbackMock::SCPIQuery(SCPICmdType cmdEnum)
{
    m_queriesReceived.append(cmdEnum);
    return "";
}
