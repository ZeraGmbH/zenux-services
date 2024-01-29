#ifndef SENSEINTERFACE_H
#define SENSEINTERFACE_H

#include "i2csettings.h"
#include "rangeadjinterface.h"
#include "senseinterfacecommon.h"
#include "systeminfo.h"
#include <QList>
#include <QHash>

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
    int rangeFlagsDevice() override;
    int rangeFlagsIntern() override;
    int rangeFlagsExtern() override;
    int rangeFlagsExternDc() override;

protected:
    void exportAdjData(QDataStream& stream, QDateTime dateTimeWrite) override;

private:
    QString getServerVersion() override;
    QString getPcbName() override;
    QString getXmlType() override;
    bool isRangePartOfAdjXmlExport(SenseRangeCommon* range) override;
    RangeAdjInterface* createJustScpiInterfaceWithAtmelPermission() override;
    QString scpiReadSenseGroupCatalog(QString& scpi) override;
    void handleScpiReadWriteMMode(cProtonetCommand* protoCmd) override;
    bool setSenseMode(QString mode);
};

#endif // SENSEINTERFACE_H
