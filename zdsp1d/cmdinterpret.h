// header datei für kommando interpreter

#ifndef KINTERPRETER_H
#define KINTERPRETER_H

#include <stdlib.h>
//#include <qstring.h>
#include "parse.h"

class cNode;
class cbIFace;

class cCmdInterpreter { // interpretiert einen inputstring und führt kommando aus (scpi)
public:
    cCmdInterpreter(cbIFace* cb, cNode* r, cParse* p);
    QString& CmdExecute(QString& sinput);
    cbIFace* m_pcbIFace; // pointer auf call back interface
    cParse* m_pParser;
private:
    QString Answer; // der vom kommando generierte output steht hier
    cNode* m_pRootCmd; // der startknoten aller kommandos
};

#endif

