// header datei für scpi parser
// start 20.12.06 Peter Lohmer

#ifndef SCPI_H
#define SCPI_H

#include <qstring.h>
#include <qstringlist.h>

#include "dsp1scpi.h"
#include "cmdinterpret.h"

class cSCPIString: public QString {
public:    
    cSCPIString() : QString(){};
    cSCPIString (const QString s) : QString(s){};
    cSCPIString (const char* s) : QString(s){};
    
    bool operator == (const QString&);  // überladen des == operators um scpi node abkürzungen auch zu erkennen
    bool  operator != (const QString& s) { return ( !(*this == s)); }; // 
    cSCPIString& operator = (const QString& s);
    cSCPIString& operator= (const char* s);
private:
    const QString ScpiAbreviation(const QString& s); 
};


enum tNodeSpec {isUnknown=0, isKnown=1, isNode=2, isCommand=4, isQuery=8} ;

class cNode
{ // rein virtuelle basisklasse
public:
    cNode(int nNodedef, cNode* pNextNode, cNode* pNewLevelNode, SCPICmdType Cmd,SCPICmdType Query);
    virtual ~cNode(){};
    virtual cNode* TestNode(cCmdInterpreter* cmdInterpreter, QChar** inputline)=0;
    int m_nNodeStat; // ergebnis des parse durchlaufes
    SCPICmdType m_nCmd;
    SCPICmdType m_nQuery;
protected:    
    cNode* m_pNextNode; // nächster knoten im selben level
    cNode* m_pNewLevelNode; // nachfolge knoten im nächsten level;
    int m_nNodeDef; // definiert was der knoten sein darf
};


class cNodeSCPI: public cNode
{
public:
    cNodeSCPI (const char* sNodeName, int nNodedef, cNode* pNextNode, cNode* pNewLevelNode, SCPICmdType Cmd, SCPICmdType Query);
    virtual  ~cNodeSCPI(){};
    virtual cNode* TestNode(cCmdInterpreter* cmdInterpreter, QChar** inputline);
private:
    cSCPIString m_sNodeName; // name des knoten
};
    
#endif

