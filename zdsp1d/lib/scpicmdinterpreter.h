#ifndef SCPICMDINTERPRETER_H
#define SCPICMDINTERPRETER_H

#include "scpiparserzdsp1d.h"

class cNode;
class cbIFace;

class ScpiCmdInterpreter
{
public:
    ScpiCmdInterpreter(cbIFace* pcbIFace, cNode* r, ScpiParserZdsp1d *parser);
    QString& CmdExecute(QString& sinput);
    cbIFace* m_pcbIFace; // pointer auf call back interface
    ScpiParserZdsp1d* m_pParser;
private:
    QString Answer; // der vom kommando generierte output steht hier
    cNode* m_pRootCmd; // der startknoten aller kommandos
};

#endif
