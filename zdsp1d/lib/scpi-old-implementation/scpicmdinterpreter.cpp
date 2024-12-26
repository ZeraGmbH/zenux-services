#include <stdlib.h>
#include <qstring.h>
#include "scpicmds.h"
#include "scpi-zdsp.h"
#include "scpicmdinterpreter.h"
#include "zscpi_response_definitions.h"

ScpiCmdInterpreter::ScpiCmdInterpreter(cbIFace* pcbIFace, cNode* r, ScpiParserZdsp1d* parser) :
    m_pcbIFace(pcbIFace),
    m_pParser(parser),
    m_pRootCmd(r)
{
}

QString &ScpiCmdInterpreter::CmdExecute(QString &sinput)
{
    Answer = ZSCPI::scpiAnswer[ZSCPI::ack];

    QChar* input = sinput.data();

    if (!input->isNull())
    { // leeres kommando ist nichts falsches -> also richtig
        QChar* CmdString = input; // der input string
        cNode* actNode = m_pRootCmd; // startknoten setzen
        cNode* prevNode;
        do
        {
            prevNode=actNode;
        } while ( (actNode = actNode->TestNode(this,&CmdString)) );
        switch ( prevNode->m_nNodeStat )
        {
        case (isKnown | isCommand) :
            Answer = m_pcbIFace->SCPICmd(prevNode->m_nCmd,CmdString);
            break;
        case (isKnown | isQuery) :
            Answer =  m_pcbIFace->SCPIQuery(prevNode->m_nQuery);
            break;
        default:
            Answer = ZSCPI::scpiAnswer[ZSCPI::nak];
        };
    }
    return (Answer);
}

