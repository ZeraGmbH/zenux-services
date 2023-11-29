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

// definition was ein client ist


cZHServer::cZHServer()
{
    sSoftwareVersion = QString("%1 %2").arg(ServerName).arg(ServerVersion);
    sServerName = ServerName;
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



