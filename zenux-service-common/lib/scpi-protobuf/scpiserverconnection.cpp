#include "scpiserverconnection.h"

ScpiServerConnection::ScpiServerConnection(const std::shared_ptr<cSCPI> &scpiInterface) :
    ScpiConnection(scpiInterface)
{
}
