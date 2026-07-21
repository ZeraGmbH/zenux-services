#ifndef SYSTEMINTERFACE_H
#define SYSTEMINTERFACE_H

#include "scpiserverconnection.h"
#include "sec1000systeminfo.h"

class cSEC1000dServer;

class Sec1000SystemInterface: public ScpiServerConnection
{
    Q_OBJECT
public:
    explicit Sec1000SystemInterface(std::shared_ptr<cSCPI> scpiInterface, cSEC1000dServer* server, Sec1000SystemInfo* sInfo);
    void initSCPIConnection() override;

private:
    void executeProtoScpi(int cmdCode, const ProtonetCommandPtr &protoCmd) override;
    QString scpiReadServerVersion(const QString &scpi);
    QString scpiReadDeviceVersion(const QString &scpi);
    QString scpiReadDeviceName(const QString &scpi);
    QString scpiReadPCBVersion(const QString &scpi);
    QString scpiReadFPGAVersion(const QString &scpi);
    QString scpiReadWriteSerialNumber(const QString &scpi);

    cSEC1000dServer* m_pMyServer = nullptr;
    Sec1000SystemInfo* m_pSystemInfo = nullptr;
};


#endif // SYSTEMINTERFACE_H
