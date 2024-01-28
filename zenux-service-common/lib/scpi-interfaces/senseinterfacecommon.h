#ifndef SENSEINTERFACECOMMON_H
#define SENSEINTERFACECOMMON_H

#include "resource.h"
#include "adjustmenteeprom.h"
#include "adjustmentxmlimportexporttemplate.h"
#include "i2csettings.h"
#include "systeminfo.h"
#include "sensechannelcommon.h"

namespace SenseSystem
{
    enum Commands
    {
        cmdVersion,
        cmdMMode,
        cmdMModeCat,
        cmdChannelCat,
        cmdGroupCat,
        initAdjData,
        computeAdjData,
        cmdStatAdjustment
    };
}

class SenseInterfaceCommon : public cResource, public AdjustmentEeprom, public AdjustmentXmlImportExportTemplate, public AdjustmentStatusInterface
{
public:
    SenseInterfaceCommon(cSCPI *scpiInterface,
                         I2cSettings* i2cSettings,
                         SystemInfo *systemInfo,
                         AbstractFactoryI2cCtrlPtr ctrlFactory);
    virtual ~SenseInterfaceCommon() = default;

protected:
    virtual void scpiReadWriteMMode(cProtonetCommand* protoCmd) = 0;

    static QString m_version;
    SystemInfo *m_systemInfo;
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    QList<SenseChannelCommon*> m_channelList;
    QString m_currSenseMode;
    QHash<QString, int> m_availSenseModesHash;
    quint8 m_nSerialStatus;

    NotificationString m_notifierSenseMMode;
    NotificationString m_notifierSenseChannelCat;
};

#endif // SENSEINTERFACECOMMON_H
