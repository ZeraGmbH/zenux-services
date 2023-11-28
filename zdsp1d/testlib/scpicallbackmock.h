#ifndef SCPICALLBACKMOCK_H
#define SCPICALLBACKMOCK_H

#include "dsp1scpi.h"
#include "dspvarparser.h"
#include <QList>

class ScpiCallbackMock : public cbIFace
{
public:
    ScpiCallbackMock(DspVarParser* parser);
    QString SCPICmd(SCPICmdType cmdEnum, QChar* scpi) override;
    QString SCPIQuery(SCPICmdType cmdEnum) override;

    struct TCommand
    {
        QString cmd;
        SCPICmdType type;
    };
    QList<TCommand> m_commandsReceived;
    QList<SCPICmdType> m_queriesReceived;
    DspVarParser* m_parser;
};

#endif // SCPICALLBACKMOCK_H
