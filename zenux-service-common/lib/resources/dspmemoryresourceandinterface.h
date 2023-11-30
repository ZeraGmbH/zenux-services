#ifndef DSPMEMORYRESOURCEANDINTERFACE_H
#define DSPMEMORYRESOURCEANDINTERFACE_H

#include "resource.h"

class DspMemoryResourceAndInterface : public cResource
{
public:
    DspMemoryResourceAndInterface(cSCPI *scpiInterface);
    void initSCPIConnection(QString leadingNodes) override;
    void registerResource(RMConnection *rmConnection, quint16 port) override;
    void unregisterResource(RMConnection *rmConnection) override;
};

#endif // DSPMEMORYRESOURCEANDINTERFACE_H
