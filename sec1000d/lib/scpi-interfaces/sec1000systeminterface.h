#ifndef SYSTEMINTERFACE_H
#define SYSTEMINTERFACE_H

#include "scpiconnection.h"

namespace SystemSystem
{

enum SystemCommands
{
    cmdVersionServer,
    cmdVersionDevice,
    cmdVersionPCB,
    cmdVersionFPGA,
    cmdSerialNumber,
    cmdInterfaceRead
};
}

class Sec1000SystemInfo;
class cSEC1000dServer;

class cSystemInterface: public ScpiConnection
{
    Q_OBJECT

public:
    cSystemInterface(std::shared_ptr<cSCPI> scpiInterface, cSEC1000dServer* server, Sec1000SystemInfo* sInfo);
    virtual void initSCPIConnection(QString leadingNodes) override;
protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
private:
    cSEC1000dServer* m_pMyServer;
    Sec1000SystemInfo* m_pSystemInfo;
    QString scpiReadServerVersion(QString& sInput);
    QString m_ReadDeviceVersion(QString& sInput);
    QString m_ReadDeviceName(QString& sInput);
    QString scpiReadPCBVersion(QString& sInput);
    QString m_ReadFPGAVersion(QString& sInput);
    QString m_ReadWriteSerialNumber(QString& sInput);
    QString m_InterfaceRead(QString& sInput);
};


#endif // SYSTEMINTERFACE_H
