#ifndef SYSTEMINTERFACE_H
#define SYSTEMINTERFACE_H

#include "com5003senseinterface.h"
#include "scpiconnection.h"
#include "pcbserver.h"
#include <QList>
#include <QJsonDocument>

namespace SystemSystem
{

enum SystemCommands
{
    cmdVersionServer,
    cmdVersionDevice,
    cmdVersionPCB,
    cmdVersionCTRL,
    cmdVersionFPGA,
    cmdSerialNumber,
    cmdAdjFlashWrite,
    cmdAdjFlashRead,
    cmdAdjXMLImportExport,
    cmdAdjXMLWrite,
    cmdAdjXMLRead,
    cmdAdjFlashChksum,
    cmdInterfaceRead
};
}

class Com5003SystemInterface: public ScpiConnection
{
    Q_OBJECT
public:
    Com5003SystemInterface(cPCBServer* server,
                           SystemInfo* sytemInfo,
                           Com5003SenseInterface *senseInterface,
                           AbstractFactoryI2cCtrlPtr ctrlFactory);
    virtual void initSCPIConnection(QString leadingNodes) override;

protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;

private:
    QString scpiReadServerVersion(QString& sInput);
    QString scpiReadAllCTRLVersions(QString& sInput);
    void updateAllCtrlVersionsJson();
    void updateAllPCBsVersion();

    QString m_ReadDeviceVersion(QString& sInput);
    QString m_ReadDeviceName(QString& sInput);
    QString m_ReadWritePCBVersion(QString& sInput);
    QString m_ReadFPGAVersion(QString& sInput);
    QString m_ReadWriteSerialNumber(QString& sInput);
    QString m_AdjFlashWrite(QString& sInput);
    QString m_AdjFlashRead(QString& sInput);
    QString m_AdjXmlImportExport(QString& sInput);
    QString m_AdjXMLWrite(QString& sInput);
    QString m_AdjXMLRead(QString& sInput);
    QString m_AdjFlashChksum(QString& sInput);
    QString m_InterfaceRead(QString& sInput);

    void m_genAnswer(int select, QString& answer);

    cPCBServer* m_pMyServer;
    SystemInfo* m_sytemInfo;
    Com5003SenseInterface *m_senseInterface;
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    NotificationString m_allCtrlVersion;
    NotificationString m_allPCBVersion;
};

#endif // SYSTEMINTERFACE_H
