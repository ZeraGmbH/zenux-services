#include "zdspglobal.h"
#include "zhserver.h"

cZHServer::cZHServer()
{
    sSoftwareVersion = QString("%1 %2").arg(ServerName).arg(ServerVersion);
    sServerName = ServerName;
}

cZHServer::~cZHServer()
{
}

QString& cZHServer::GetSoftwareVersion()
{ // abfrage software version
    return (sSoftwareVersion);
}
