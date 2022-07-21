#ifndef STATUSINTERFACE_H
#define STATUSINTERFACE_H

#include <QObject>
#include <QList>
#include <scpi.h>

#include "sec1000d.h"
#include <scpiconnection.h>
#include <scpidelegate.h>

namespace StatusSystem
{

enum StatusCommands
{
    cmdDevice,
};
}


class cStatusInterface: public cSCPIConnection
{
    Q_OBJECT

public:
    cStatusInterface();
    virtual void initSCPIConnection(QString leadingNodes) override;

protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd) override;

private:
    quint16 getDeviceStatus();
};

#endif // STATUSINTERFACE_H
