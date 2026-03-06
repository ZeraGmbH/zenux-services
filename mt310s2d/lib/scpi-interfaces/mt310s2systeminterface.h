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
    QString scpiReadServerVersion(const QString &scpi);
    QString scpiReadAllCTRLVersions(const QString& scpi);
    void triggerVersionInfoChanges();
    void updateAllCtrlVersionsJson();
    void updateAllPCBsVersion();
    QString readChannelsConnected();

    QString scpiReadDeviceVersion(const QString& scpi);
    QString scpiReadDeviceName(const QString& scpi);
    QString scpiReadPCBVersion(const QString& scpi);
    QString scpiReadFPGAVersion(const QString& scpi);
    QString scpiReadWriteSerialNumber(const QString& scpi);
    QString scpiAdjFlashWrite(const QString& scpi);
    QString scpiAdjFlashRead(const QString& scpi);
    QString scpiAdjXmlImportExport(const QString& scpi);
    QString scpiAdjXMLWrite(const QString& scpi);
    QString scpiAdjXMLRead(const QString& scpi);
    QString scpiAdjFlashChksum(const QString& scpi);

    void genAnswer(int select, QString& answer);

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
