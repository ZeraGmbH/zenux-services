#ifndef SCPISERVERCONNECTION_H
#define SCPISERVERCONNECTION_H

#include "scpiconnection.h"

class ScpiServerConnection : public ScpiConnection
{
    Q_OBJECT
public:
    ScpiServerConnection(const std::shared_ptr<cSCPI> &scpiInterface);
    virtual void initSCPIConnection() = 0;
};

#endif // SCPISERVERCONNECTION_H
