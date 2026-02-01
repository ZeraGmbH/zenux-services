#ifndef MT310S2SYSTEMINTERFACE_H
#define MT310S2SYSTEMINTERFACE_H

#include "scpiserverconnection.h"
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
    cmdGetChannels,
};
}

class Mt310s2SystemInterface: public ScpiServerConnection
{
    Q_OBJECT

public:
    Mt310s2SystemInterface(PCBServer* server,
                           Mt310s2SystemInfo *systemInfo,
                           cSenseSettingsPtr senseSettings,
                           SenseInterfaceCommon *senseInterface,
                           AbstractFactoryI2cCtrlPtr ctrlFactory,
                           HotPluggableControllerContainerPtr hotPluggableControllerContainer);
    void initSCPIConnection() override;
public slots:
    void onAccuStatusChanged(uint8_t status);
protected:
    void executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd) override;
private:
    QString scpiReadServerVersion(QString& sInput);
    QString scpiReadAllCTRLVersions(QString& sInput);
    void triggerVersionInfoChanges();
    void updateAllCtrlVersionsJson();
    void updateAllPCBsVersion();
    QString readChannelsConnected();

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

    void m_genAnswer(int select, QString& answer);

    PCBServer* m_pMyServer;
    Mt310s2SystemInfo *m_systemInfo;
    cSenseSettingsPtr m_senseSettings;
    SenseInterfaceCommon* m_senseInterface;
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    HotPluggableControllerContainerPtr m_hotPluggableControllerContainer;

    NotificationString m_allCtrlVersion;
    NotificationString m_allPCBVersion;
    bool m_currAccuPlugged = false;

    TimerTemplateQtPtr m_delayedChangeTriggerForMissingAccuVersionTimer;
    bool m_initialDelayTriggerDone = false;
};


#endif // MT310S2SYSTEMINTERFACE_H
