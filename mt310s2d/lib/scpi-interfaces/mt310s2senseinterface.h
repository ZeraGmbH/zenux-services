#ifndef MT310S2SENSEINTERFACE_H
#define MT310S2SENSEINTERFACE_H

#include "senseinterfacecommon.h"

class Mt310s2SenseInterface : public SenseInterfaceCommon
{
    Q_OBJECT
public:
    Mt310s2SenseInterface(std::shared_ptr<cSCPI> scpiInterface,
                          I2cSettings *i2cSettings,
                          cSenseSettings *senseSettings,
                          SystemInfo *systemInfo,
                          AbstractFactoryI2cCtrlPtr ctrlFactory);
// MT specifics only
    static QList<SenseChannelCommon*> setChannelAndRanges(cSenseSettings* senseSettings,
                                                           AdjDataPtr adjData,
                                                           std::shared_ptr<cSCPI> scpi,
                                                           AbstractFactoryI2cCtrlPtr ctrlFactory);

private:
    const char* getAdjExportedVersion() override;
    QString getPcbName() override;
    QString getXmlType() override;
    bool isRangePartOfAdjXmlExport(SenseRangeCommon* range) override;
    static AdjRangeScpi* createJustScpiInterfaceWithAtmelPermission(std::shared_ptr<cSCPI> scpi);
    QString scpiReadSenseGroupCatalog(QString& scpi) override;
    bool setSenseMode(QString mode) override;
};

#endif // MT310S2SENSEINTERFACE_H
