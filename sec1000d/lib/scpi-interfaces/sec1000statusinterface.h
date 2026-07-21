#ifndef STATUSINTERFACE_H
#define STATUSINTERFACE_H

#include "scpiserverconnection.h"

class Sec1000StatusInterface: public ScpiServerConnection
{
    Q_OBJECT
public:
    explicit Sec1000StatusInterface(std::shared_ptr<cSCPI> scpiInterface);
    void initSCPIConnection() override;

private:
    void executeProtoScpi(int cmdCode, const ProtonetCommandPtr &protoCmd) override;
    quint16 getDeviceStatus();
};

#endif // STATUSINTERFACE_H
