// headerdatei zera hardware server 
// basis klasse für einen solchen
// rein virtuell -> definiert was ein hw-server
// immer benötigt

#ifndef ZHSERVER_H
#define ZHSERVER_H

#include "dsp1scpi.h"
#include "cmdinterpret.h"
#include <netinet/in.h>
#include <QStringList>
#include <QString>

class cZHServer
{
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
