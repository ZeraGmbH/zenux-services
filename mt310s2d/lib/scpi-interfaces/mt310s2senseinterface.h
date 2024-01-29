#ifndef SENSEINTERFACE_H
#define SENSEINTERFACE_H

#include "i2csettings.h"
#include "adjustmenteeprom.h"
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
    QString exportXMLString(int indent = 1) override;
    int rangeFlagsDevice() override;
    int rangeFlagsIntern() override;
    int rangeFlagsExtern() override;
    int rangeFlagsExternDc() override;

protected:
    void exportAdjData(QDataStream& stream, QDateTime dateTimeWrite) override;
    bool importAdjData(QDataStream& stream) override;
    bool importXMLDocument(QDomDocument* qdomdoc) override;

private:
    // MT specifics
    RangeAdjInterface* createJustScpiInterfaceWithAtmelPermission() override;
    QString scpiReadSenseGroupCatalog(QString& scpi) override;
    void handleScpiReadWriteMMode(cProtonetCommand* protoCmd) override;
    bool setSenseMode(QString mode);
};

#endif // SENSEINTERFACE_H
