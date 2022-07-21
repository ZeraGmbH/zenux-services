// headerdatei zera hardware server 
// basis klasse für einen solchen
// rein virtuell -> definiert was ein hw-server
// immer benötigt

// cNodeZHServer ist eine erweiterung zum navigieren in scpi (ähnlichen)
// befehlslisten 

#ifndef ZHSERVER_H
#define ZHSERVER_H

#include <netinet/in.h>
#include <QStringList>
#include <QString>

#include "scpi.h"
#include "dsp1scpi.h"
#include "cmdinterpret.h"


namespace Zera
{
    namespace Net
    {
        class cServer;
        class cClient;
    }
    namespace XMLConfig
    {
        class cReader;
    }
}


class cNodeZHServer: public cNode {
public:
    cNodeZHServer(QStringList*,QString*,int,cNode*,cNode*,SCPICmdType,SCPICmdType); 
    virtual ~cNodeZHServer(){}
    // konstruktor, psNodeNames,psNode2Set, nNodedef, pNextNode, pNewLevelNode, Cmd, Query
    virtual cNode* TestNode(cCmdInterpreter*, QChar **); // zeiger, zeiger auf zeiger auf inputzeile, testet den knoten
    void SetNodeNameList(QStringList*); // zum späteren umschreiben der liste der knotennamen
private:
    QStringList* sNodeNames; // liste der möglichen nodes (es handelt sich z.b. um kanal namen)
    QString* psNode2Set; // der gefundene knoten name wird nach psNode2Set kopiert für weitere bearbeitung
};


class cZHServer {
public:
    cZHServer();
    cZHServer(cCmdInterpreter*);
    virtual ~cZHServer();
    QString& GetSoftwareVersion();

protected:
    QString sServerName;
    int ActSock; // der aktive socket im Execute
    QString sSoftwareVersion; // version des hw-servers (programm name + version )
    cCmdInterpreter* pCmdInterpreter; // der benutzte kommando interpreter
};    
    
#endif // ifndef ZHSERVER_H
