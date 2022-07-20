// implementation zhserver

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <qstring.h>
#include <qstringlist.h>

#include "zdspglobal.h"
#include "zhserver.h"
#include "dsp1scpi.h"

class cNode;

// hier erst mal noch einen speziellen kommando knoten typ für den befehlsinterpreter 
// konstruktor, psNodeNames, psNode2Set,nNodeDef, pNextNode, pNewLevelNode, m_pCmd, m_pQuery 
cNodeZHServer::cNodeZHServer(QStringList* sl,QString* s,int ns,cNode* n1,cNode* n2,SCPICmdType ct,SCPICmdType qt)
    :cNode(ns,n1,n2,ct,qt)
{
    
    sNodeNames=sl;
    psNode2Set=s;
}


cNode* cNodeZHServer::TestNode(cCmdInterpreter* ci,QChar** inp)
{
    QChar* tinp=*inp;
    QString stmp;
    stmp=ci->m_pParser->GetKeyword(&tinp); // lässt das nächste schlüsselwort vom parser lesen
    QString STMP = stmp.toUpper();
    m_nNodeStat=isUnknown; // erst mal unbekannt
    for ( QStringList::iterator it = sNodeNames->begin(); it != sNodeNames->end(); ++it )
    {
        if ( ((*it) == stmp)  || ((*it) == STMP))
        {
            *psNode2Set=*it; // speichere knotenname an der vorgesehen stelle
            m_nNodeStat |= isKnown;
            break;
        }
    }

    if (m_nNodeStat == isUnknown) return (pNextNode); // zum prüfen des nächsten knoten ( solange != NULL gehts weiter )
    *inp=tinp; // hinter dem schlüsselwort gehts weiter
    
    char c = ci->m_pParser->GetChar(inp).toLatin1();

    switch (c)
    {
    case ':' 	: // es ist ein knoten
        if (nNodeDef & isNode) {
            m_nNodeStat |= isNode; // es darf einer sein
            if (m_nQuery) ci->m_pcbIFace->SCPIQuery(m_nQuery); // in abhängigkeit vom knoten muss noch was passieren (z.b. einen nachfolgenden knoten umbauen)
            return (pNewLevelNode); // dann return nächsten level ( d.h. es geht weiter )
        }
        else return (NULL); // es darf keiner sein -> fertig !!!
    case '?'	: // es ist eine query
        m_nNodeStat |= (nNodeDef & isQuery); // gesetzt wenn es eine sein darf
        return (NULL); // -> fertig !!!
    default		: // es ist ein command
        m_nNodeStat |= (nNodeDef & isCommand); // gesetzt wenn es eines sein darf
        *inp=tinp; // lass dem Kommando das zeichen (reparsen)
        return (NULL);
    }
}


void cNodeZHServer::SetNodeNameList(QStringList* sl)
{ // umschreiben der liste der knotennamen
    sNodeNames=sl;
}

// definition was ein client ist


cZHServer::cZHServer()
{
    sSoftwareVersion = QString("%1 %2").arg(ServerBasisName).arg(ServerVersion);
    sServerName = ServerBasisName;
}


cZHServer::cZHServer(cCmdInterpreter* ci)
{
    cZHServer();
    pCmdInterpreter=ci;
}


cZHServer::~cZHServer()
{
}


QString& cZHServer::GetSoftwareVersion()
{ // abfrage software version
    return (sSoftwareVersion);
}



