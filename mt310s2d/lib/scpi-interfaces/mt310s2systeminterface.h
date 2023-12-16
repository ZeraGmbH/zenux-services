#ifndef SYSTEMINTERFACE_H
#define SYSTEMINTERFACE_H

#include "scpiconnection.h"
#include "hotpluggablecontrollercontainer.h"
#include <QList>
#include <QJsonDocument>
#include <memory>

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
    cmdUpdateControlerBootloader,
    cmdUpdateControlerProgram,
    cmdUpdateControlerFlash,
    cmdUpdateControlerEEprom,
    cmdAdjFlashWrite,
    cmdAdjFlashRead,
    cmdAdjXMLImportExport,
    cmdAdjXMLWrite,
    cmdAdjXMLRead,
    cmdAdjFlashChksum,
    cmdInterfaceRead,
    cmdTestMode
};
}

class cMT310S2dServer;

class Mt310s2SystemInterface: public ScpiConnection
{
    Q_OBJECT

public:
    Mt310s2SystemInterface(cMT310S2dServer* server, HotPluggableControllerContainerPtr hotPluggableControllerContainer);
    virtual void initSCPIConnection(QString leadingNodes) override;
    void actualizeContollers(quint16 bitmaskAvailable);

protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
private slots:
    void onHotPluggableContainersChanged();
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
    QString m_StartControlerBootloader(QString& sInput);
    QString m_StartControlerProgram(QString& sInput);
    QString m_LoadFlash(QString& sInput);
    QString m_LoadEEProm(QString& sInput);
    QString m_AdjFlashWrite(QString& sInput);
    QString m_AdjFlashRead(QString& sInput);
    QString m_AdjXmlImportExport(QString& sInput);
    QString m_AdjXMLWrite(QString& sInput);
    QString m_AdjXMLRead(QString& sInput);
    QString m_AdjFlashChksum(QString& sInput);
    QString m_InterfaceRead(QString& sInput);
    QString testMode(QString &Input);

    void m_genAnswer(int select, QString& answer);

    cMT310S2dServer* m_pMyServer;
    HotPluggableControllerContainerPtr m_hotPluggableControllerContainer;
    NotificationString m_allCtrlVersion;
    NotificationString m_allPCBVersion;
};


#endif // SYSTEMINTERFACE_H
