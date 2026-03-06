#ifndef SYSTEMINTERFACE_H
#define SYSTEMINTERFACE_H

#include "scpiserverconnection.h"

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

class cSystemInterface: public ScpiServerConnection
{
    Q_OBJECT

public:
    cSystemInterface(std::shared_ptr<cSCPI> scpiInterface, cSEC1000dServer* server, Sec1000SystemInfo* sInfo);
    void initSCPIConnection() override;
protected:
    void executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd) override;
private:
    cSEC1000dServer* m_pMyServer;
    Sec1000SystemInfo* m_pSystemInfo;
    QString scpiReadServerVersion(const QString &scpi);
    QString scpiReadDeviceVersion(const QString &scpi);
    QString scpiReadDeviceName(const QString &scpi);
    QString scpiReadPCBVersion(const QString &scpi);
    QString scpiReadFPGAVersion(const QString &scpi);
    QString scpiReadWriteSerialNumber(const QString &scpi);
};


#endif // SYSTEMINTERFACE_H
