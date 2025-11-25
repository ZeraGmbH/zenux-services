#ifndef COM5003SENSEINTERFACE_H
#define COM5003SENSEINTERFACE_H

#include "senseinterfacecommon.h"

class Com5003SenseInterface : public SenseInterfaceCommon
{
    Q_OBJECT
public:
    // COM specifics only
    Com5003SenseInterface(std::shared_ptr<cSCPI> scpiInterface,
                          cSenseSettings* senseSettings,
                          EepromI2cDeviceInterfacePtr adjMemory,
                          SystemInfo *systemInfo,
                          AbstractChannelRangeFactoryPtr rangeFactory,
                          AbstractFactoryI2cCtrlPtr ctrlFactory);

private:
    void changeSense();
    const char* getAdjExportedVersion() override;
    QString getPcbName() override;
    QString getXmlType() override;
    bool isRangePartOfAdjXmlExport(SenseRangeCommon* range) override;
    QString scpiReadSenseGroupCatalog(const QString& scpi) override;
    bool setSenseMode(const QString &modeStr) override;
};

#endif // COM5003SENSEINTERFACE_H
