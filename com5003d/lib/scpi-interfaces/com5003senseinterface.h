#ifndef COM5003SENSEINTERFACE_H
#define COM5003SENSEINTERFACE_H

#include "senseinterfacecommon.h"

class Com5003SenseInterface : public SenseInterfaceCommon
{
    Q_OBJECT
public:
    Com5003SenseInterface(std::shared_ptr<cSCPI> scpiInterface,
                          I2cSettings* i2cSettings,
                          cSenseSettings* senseSettings,
                          SystemInfo *systemInfo,
                          AbstractFactoryI2cCtrlPtr ctrlFactory);
// COM specifics only
    static QList<SenseChannelCommon*> setChannelAndRanges(cSenseSettings *senseSettings,
                                                           AdjDataPtr adjData,
                                                           std::shared_ptr<cSCPI> scpi,
                                                           AbstractFactoryI2cCtrlPtr ctrlFactory);

private:
    void changeSense();
    const char* getAdjExportedVersion() override;
    QString getPcbName() override;
    QString getXmlType() override;
    bool isRangePartOfAdjXmlExport(SenseRangeCommon* range) override;
    AdjRangeScpi* createJustScpiInterfaceWithAtmelPermission();
    QString scpiReadSenseGroupCatalog(QString& scpi) override;
    void handleScpiReadWriteMMode(cProtonetCommand* protoCmd) override;
    bool setSenseMode(QString modeStr);
};

#endif // COM5003SENSEINTERFACE_H
