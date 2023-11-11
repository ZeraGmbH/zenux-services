// implementation scpi

#include <qstring.h>

#include "scpi-zdsp.h"
#include "parse.h"


bool cSCPIString::operator == (const QString& s)
{
    QString tmp=*this; // mach einen qstring daraus
    QString atmp=ScpiAbreviation(tmp); // scpi konforme abkürzung davon
    QString S=s.toUpper(); // das ganze in gross
    bool b=( (tmp==s) || (tmp == S) || (atmp==s) || (atmp == S) );

    return (b);
}


cSCPIString& cSCPIString::operator = (const QString& s)
{
    *this=(cSCPIString) s;

    return (*this);
}

cSCPIString& cSCPIString::operator = (const char* s)
{
    *this=(cSCPIString) s;

    return (*this);
}

const QString cSCPIString::ScpiAbreviation (const QString& s)
{
    if (s.length() <= 4) return s; // wenn lange kommando form <= 4 zeichen -> kurzes kommando = langes kommando
    QString t=s.left(4); // kurzes kommando ist erste 4 zeichen
    if ( QString("AEIOU").contains(t.right(1),Qt::CaseInsensitive) ) t=t.left(3); // ist das 4. zeichen ein vokal -> kurzes kommando ist erste 3 zeichen

    return(t);
}
    
cNode::cNode(int nNodedef, cNode* pNextNode, cNode* n2, SCPICmdType cmd, SCPICmdType Query)
{
    m_nNodeDef = nNodedef;
    m_pNextNode = pNextNode;
    m_pNewLevelNode = n2;
    m_nCmd = cmd;
    m_nQuery = Query;
}

cNodeSCPI::cNodeSCPI (const char* sNodeName, int nNodedef, cNode* pNextNode, cNode* pNewLevelNode, SCPICmdType Cmd, SCPICmdType Query) :
    cNode(nNodedef, pNextNode, pNewLevelNode, Cmd, Query)
{
    m_sNodeName=sNodeName;
}


cNode* cNodeSCPI::TestNode(cCmdInterpreter* cmdInterpreter, QChar **inputline)
{
    QChar* tinp=*inputline; // zeiger kopieren
    QString stmp=cmdInterpreter->m_pParser->GetKeyword(&tinp); // lässt das nächste schlüsselwort vom parser lesen ( der ist static)
    m_nNodeStat=isUnknown; // erst mal unbekannt
    if (m_sNodeName != stmp) return (m_pNextNode); // zum prüfen des nächsten knoten ( solange != NULL gehts weiter )
    m_nNodeStat |= isKnown;
    *inputline=tinp; // hinter dem schlüsselwort gehts weiter
    
    char c = cmdInterpreter->m_pParser->GetChar(inputline).toLatin1();
    switch (c)
    {
    case ':' 	: // es ist ein knoten
        if (m_nNodeDef & isNode)
        {
            m_nNodeStat |= isNode; // es darf einer sein
            return (m_pNewLevelNode); // dann return nächsten level ( d.h. es geht weiter )
        }
        else
            return (NULL); // es darf keiner sein -> fertig !!!
    case '?'	: // es ist eine query
        m_nNodeStat |= (m_nNodeDef & isQuery); // gesetzt wenn es eine sein darf
        return (NULL); // -> fertig !!!
    default		: // es ist ein command
        m_nNodeStat |= (m_nNodeDef & isCommand); // gesetzt wenn es eines sein darf
        *inputline=tinp; // lass dem Kommando das zeichen (reparsen)
        return (NULL);
    }
}


