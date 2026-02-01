#ifndef STATUSINTERFACE_H
#define STATUSINTERFACE_H

#include "sec1000d.h"
#include "scpiserverconnection.h"
#include "scpidelegate.h"
#include <QString>

namespace StatusSystem
{

enum StatusCommands
{
    cmdDevice,
};
}


class Sec1000StatusInterface: public ScpiServerConnection
{
    Q_OBJECT
public:
    Sec1000StatusInterface(std::shared_ptr<cSCPI> scpiInterface);
    void initSCPIConnection() override;
protected:
    void executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd) override;
private:
    quint16 getDeviceStatus();
};

#endif // STATUSINTERFACE_H
