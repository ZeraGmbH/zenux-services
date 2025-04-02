#ifndef MT310S2SENSEINTERFACE_H
#define MT310S2SENSEINTERFACE_H

#include "senseinterfacecommon.h"

class Mt310s2SenseInterface : public SenseInterfaceCommon
{
    Q_OBJECT
public:
    // MT specifics only
    Mt310s2SenseInterface(std::shared_ptr<cSCPI> scpiInterface,
                          I2cSettings *i2cSettings,
                          cSenseSettings *senseSettings,
                          SystemInfo *systemInfo,
                          AbstractFactoryI2cCtrlPtr ctrlFactory);

private:
    const char* getAdjExportedVersion() override;
    QString getPcbName() override;
    QString getXmlType() override;
    bool isRangePartOfAdjXmlExport(SenseRangeCommon* range) override;
    QString scpiReadSenseGroupCatalog(const QString& scpi) override;
    bool setSenseMode(QString mode) override;
};

#endif // MT310S2SENSEINTERFACE_H
