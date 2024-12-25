#ifndef STATUSINTERFACE_H
#define STATUSINTERFACE_H

#include "sec1000d.h"
#include "scpiconnection.h"
#include "scpidelegate.h"
#include <QString>

namespace StatusSystem
{

enum StatusCommands
{
    cmdDevice,
};
}


class Sec1000StatusInterface: public ScpiConnection
{
    Q_OBJECT
public:
    Sec1000StatusInterface(std::shared_ptr<cSCPI> scpiInterface);
    virtual void initSCPIConnection(QString leadingNodes) override;
protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
private:
    quint16 getDeviceStatus();
};

#endif // STATUSINTERFACE_H
