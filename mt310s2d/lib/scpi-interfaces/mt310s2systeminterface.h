#ifndef SYSTEMINTERFACE_H
#define SYSTEMINTERFACE_H

#include "scpiconnection.h"
#include "hotpluggablecontrollercontainer.h"
#include "pcbserver.h"
#include "senseinterfacecommon.h"
#include "mt310s2systeminfo.h"

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
};
}

class Mt310s2SystemInterface: public ScpiConnection
{
    Q_OBJECT

public:
    Mt310s2SystemInterface(cPCBServer* server,
                           Mt310s2SystemInfo *systemInfo,
                           cSenseSettings *senseSettings,
                           SenseInterfaceCommon *senseInterface,
                           AbstractFactoryI2cCtrlPtr ctrlFactory,
                           HotPluggableControllerContainerPtr hotPluggableControllerContainer);
    virtual void initSCPIConnection(QString leadingNodes) override;
    void actualizeContollers(quint16 bitmaskAvailable);
public slots:
    void onAccuStatusChanged(uint8_t status);
protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
private:
    QString scpiReadServerVersion(QString& sInput);
    QString scpiReadAllCTRLVersions(QString& sInput);
    void triggerVersionInfoChanges();
    void updateAllCtrlVersionsJson();
    void updateAllPCBsVersion();

    QString m_ReadDeviceVersion(QString& sInput);
    QString m_ReadDeviceName(QString& sInput);
    QString scpiReadPCBVersion(QString& sInput);
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
    Mt310s2SystemInfo *m_systemInfo;
    cSenseSettings *m_senseSettings;
    SenseInterfaceCommon* m_senseInterface;
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    HotPluggableControllerContainerPtr m_hotPluggableControllerContainer;

    NotificationString m_allCtrlVersion;
    NotificationString m_allPCBVersion;
    bool m_currAccuPlugged = false;

    TimerTemplateQtPtr m_delayedChangeTriggerForMissingAccuVersionTimer;
    bool m_initialDelayTriggerDone = false;
};


#endif // SYSTEMINTERFACE_H
