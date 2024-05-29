#ifndef SENSEINTERFACE_H
#define SENSEINTERFACE_H

#include "senseinterfacecommon.h"

class Mt310s2SenseInterface : public SenseInterfaceCommon
{
    Q_OBJECT
public:
    Mt310s2SenseInterface(cSCPI *scpiInterface,
                          I2cSettings *i2cSettings,
                          cSenseSettings *senseSettings,
                          SystemInfo *systemInfo,
                          AbstractFactoryI2cCtrlPtr ctrlFactory);
// MT specifics only
    void setChannelAndRanges(cSenseSettings* senseSettings);
    int rangeFlagsExtern() override;
    int rangeFlagsExternDc() override;

private:
    int rangeFlagsDevice();
    int rangeFlagsIntern();
    const char* getAdjExportedVersion() override;
    QString getPcbName() override;
    QString getXmlType() override;
    bool isRangePartOfAdjXmlExport(SenseRangeCommon* range) override;
    AdjRangeInterface* createJustScpiInterfaceWithAtmelPermission();
    QString scpiReadSenseGroupCatalog(QString& scpi) override;
    void handleScpiReadWriteMMode(cProtonetCommand* protoCmd) override;
    bool setSenseMode(QString mode);
};

#endif // SENSEINTERFACE_H
