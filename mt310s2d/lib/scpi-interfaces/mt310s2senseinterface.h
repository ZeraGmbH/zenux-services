#ifndef SENSEINTERFACE_H
#define SENSEINTERFACE_H

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
    int rangeFlagsExtern() override;
    int rangeFlagsExternDc() override;

private:
    const char* getAdjExportedVersion() override;
    QString getPcbName() override;
    QString getXmlType() override;
    bool isRangePartOfAdjXmlExport(SenseRangeCommon* range) override;
    static AdjRangeScpi* createJustScpiInterfaceWithAtmelPermission(std::shared_ptr<cSCPI> scpi);
    QString scpiReadSenseGroupCatalog(QString& scpi) override;
    void handleScpiReadWriteMMode(cProtonetCommand* protoCmd) override;
    bool setSenseMode(QString mode);
};

#endif // SENSEINTERFACE_H
