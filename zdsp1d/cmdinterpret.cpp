// kommando interpreter für scpi kommandolisten

#include <stdlib.h>
#include <qstring.h>
#include "zeraglobal.h"
#include "dsp1scpi.h"
#include "scpi.h"
#include "cmdinterpret.h"


cCmdInterpreter::cCmdInterpreter(cbIFace* cb, cNode* r, cParse* p)
    :m_pcbIFace(cb), m_pParser(p), m_pRootCmd(r)
{
}


QString &cCmdInterpreter::CmdExecute(QString &sinput)
{
    Answer = ACKString;

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
        case (isKnown | isCommand) : Answer = m_pcbIFace->SCPICmd(prevNode->m_nCmd,CmdString);break;
        case (isKnown | isQuery) : Answer =  m_pcbIFace->SCPIQuery(prevNode->m_nQuery);break;
        default: Answer = NACKString;
        };
    }
    return (Answer);
}

