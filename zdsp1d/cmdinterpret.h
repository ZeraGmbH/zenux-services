#ifndef KINTERPRETER_H
#define KINTERPRETER_H

#include "dspvarparser.h"

class cNode;
class cbIFace;

class cCmdInterpreter { // interpretiert einen inputstring und führt kommando aus (scpi)
public:
    cCmdInterpreter(cbIFace* cb, cNode* r, DspVarParser *p);
    QString& CmdExecute(QString& sinput);
    cbIFace* m_pcbIFace; // pointer auf call back interface
    DspVarParser* m_pParser;
private:
    QString Answer; // der vom kommando generierte output steht hier
    cNode* m_pRootCmd; // der startknoten aller kommandos
};

#endif

