#ifndef SENSEINTERFACECOMMON_H
#define SENSEINTERFACECOMMON_H

#include "resource.h"
#include "adjustmenteeprom.h"
#include "adjustmentxmlimportexporttemplate.h"
#include "i2csettings.h"

namespace SenseSystem
{
    const QString Version = "V1.00";
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
    SenseInterfaceCommon(cSCPI *scpiInterface, I2cSettings* i2cSettings);
    virtual ~SenseInterfaceCommon() = default;
};

#endif // SENSEINTERFACECOMMON_H
