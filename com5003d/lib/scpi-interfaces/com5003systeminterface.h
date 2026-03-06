#ifndef COM5003SYSTEMINTERFACE_H
#define COM5003SYSTEMINTERFACE_H

#include "senseinterfacecommon.h"
#include "scpiserverconnection.h"
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
    cmdInterfaceRead,
    cmdGetChannels,
};
}

class Com5003SystemInterface: public ScpiServerConnection
{
    Q_OBJECT
public:
    Com5003SystemInterface(PCBServer* server,
                           SystemInfo* systemInfo,
                           SenseInterfaceCommon *senseInterface,
                           AbstractFactoryI2cCtrlPtr ctrlFactory);
    void initSCPIConnection() override;

protected:
    void executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd) override;

private:
    QString scpiReadServerVersion(const QString &scpi);
    QString scpiReadAllCTRLVersions(const QString &scpi);
    void updateAllCtrlVersionsJson();
    void updateAllPCBsVersion();

    QString scpiReadDeviceVersion(const QString& scpi);
    QString scpiReadDeviceName(const QString &scpi);
    QString scpiReadPCBVersion(const QString &scpi);
    QString scpiReadFPGAVersion(const QString &scpi);
    QString scpiReadWriteSerialNumber(const QString &scpi);
    QString scpiAdjFlashWrite(const QString &scpi);
    QString scpiAdjFlashRead(const QString &scpi);
    QString scpiAdjXmlImportExport(const QString &scpi);
    QString scpiAdjXMLWrite(const QString &scpi);
    QString scpiAdjXMLRead(QString &scpi);
    QString scpiAdjFlashChksum(QString &scpi);

    void genAnswer(int select, QString& answer);

    PCBServer* m_pMyServer;
    SystemInfo* m_systemInfo;
    SenseInterfaceCommon *m_senseInterface;
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    NotificationString m_allCtrlVersion;
    NotificationString m_allPCBVersion;
};

#endif // COM5003SYSTEMINTERFACE_H
