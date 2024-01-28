#ifndef SENSEINTERFACE_H
#define SENSEINTERFACE_H

#include "i2csettings.h"
#include "adjustmenteeprom.h"
#include "rangeadjinterface.h"
#include "sensechannelcommon.h"
#include "senseinterfacecommon.h"
#include "systeminfo.h"
#include <QList>
#include <QHash>

namespace SenseSystem
{
    enum MMode
    {
        // This is protection of intellectual property by obscurity at it's best.
        // To shed some light see:
        //
        // * test_regression_sense_interface_mt310s2: all the tests with mode setting
        // * Mt310s2SenseInterface::Mt310s2SenseInterface on setup of m_availSenseModesHash
        // * SenseRangeCommon::setMMode for sense range (de)activation
        // * Mt310s2SenseInterface::setSenseMode to see some nasty bitmanipulation
        //   before asking atmel to change mode.
        //
        // To make things even worse: COM5003 has other ideas on modes (seach for
        // 'enum MMode' and 'Com5003SenseInterface::registerSense()') and howto handle
        // mode change...

        // ranges with this are active on mmode 'AC' (standard mode)
        modeAC = 1,

        // ranges with this are active on mmode 'HF' At the time of writing no range supporst this!!!
        // Maybe more ingenious concepts never finished and causing questions now?
        modeHF = 2,

        // there is no mmode for ranges with this - we could remove that from DC clamps or implement some
        // superclever code using this flag???
        modeDC = 4,

        // ranges with this are active on mmode 'ADJ'. This is used to enable internal clamp (voltage-)ranges
        modeADJ = 8
    };
    enum SensorType
    {
        Direct = 0x100,
        Clamp = 0x200
    };
    const QString sVoltageChannelDescription = "Measuring channel 0..250V 50Hz/150kHz";
    const QString sCurrentChannelDescription = "Measuring channel 0..1000A 50Hz/150kHz";
    const QString sMeasuringModeDescription = "Measuring mode switch AC,HF,ADJ";
}

class Mt310s2SenseInterface : public SenseInterfaceCommon
{
    Q_OBJECT
public:
    Mt310s2SenseInterface(cSCPI *scpiInterface,
                          I2cSettings *i2cSettings,
                          cSenseSettings *senseSettings,
                          SystemInfo *systemInfo,
                          AbstractFactoryI2cCtrlPtr ctrlFactory);
    ~Mt310s2SenseInterface();
    virtual void initSCPIConnection(QString leadingNoMModedes) override;
    SenseChannelCommon *getChannel(QString& name);
    QString getChannelSystemName(quint16 ctrlChannel);
    SenseRangeCommon* getRange(QString channelName, QString rangeName);
    quint8 getAdjustmentStatus() override;

    QString exportXMLString(int indent = 1) override;

    void registerResource(RMConnection *rmConnection, quint16 port) override;
    void computeSenseAdjData();

protected:
    void exportAdjData(QDataStream& stream, QDateTime dateTimeWrite) override;
    bool importAdjData(QDataStream& stream) override;
    bool importXMLDocument(QDomDocument* qdomdoc) override;
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;

private:
    QString scpiReadVersion(QString& scpi);
    QString m_ReadMModeCatalog(QString& scpi);
    QString m_ReadSenseChannelCatalog(QString& sInput);
    QString m_ReadSenseGroupCatalog(QString& sInput);
    QString m_InitSenseAdjData(QString& sInput);
    QString m_ComputeSenseAdjData(QString& sInput);
    QString scpiReadAdjStatus(QString& sInput);

    // MT specifics
    RangeAdjInterface* createJustScpiInterfaceWithAtmelPermission() override;
    void scpiReadWriteMMode(cProtonetCommand* protoCmd) override;
    bool setSenseMode(QString mode);
};

#endif // SENSEINTERFACE_H
