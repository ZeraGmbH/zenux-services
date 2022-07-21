#ifndef SYSTEMINTERFACE_H
#define SYSTEMINTERFACE_H

#include <QObject>
#include <QList>
#include <scpi.h>

#include <scpiconnection.h>

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

class cSystemInfo;
class cSEC1000dServer;
class cAdjustment;

class cSystemInterface: public cSCPIConnection
{
    Q_OBJECT

public:
    cSystemInterface(cSEC1000dServer* server, cSystemInfo* sInfo);
    virtual void initSCPIConnection(QString leadingNodes) override;

protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd) override;

private:
    cSEC1000dServer* m_pMyServer;
    cSystemInfo* m_pSystemInfo;
    QString m_ReadServerVersion(QString& sInput);
    QString m_ReadDeviceVersion(QString& sInput);
    QString m_ReadDeviceName(QString& sInput);
    QString m_ReadWritePCBVersion(QString& sInput);
    QString m_ReadFPGAVersion(QString& sInput);
    QString m_ReadWriteSerialNumber(QString& sInput);
    QString m_InterfaceRead(QString& sInput);
};


#endif // SYSTEMINTERFACE_H
