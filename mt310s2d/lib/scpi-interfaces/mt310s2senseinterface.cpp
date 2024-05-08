#include "mt310s2senseinterface.h"
#include "mt310s2dglobal.h"
#include "adjustmentflags.h"
#include "mt310s2sensechannel.h"
#include "mt310s2senserange.h"
#include "protonetcommand.h"
#include "rangeadjinterface.h"
#include "scpiconnection.h"
#include "zscpi_response_definitions.h"
#include "i2csettings.h"
#include "sensesettings.h"
#include <xmlsettings.h>
#include <QStringList>
#include <QDomDocument>
#include <QDateTime>
#include <QDebug>
#include <QFile>

enum SensorType {
    Direct = 0x100,
    Clamp = 0x200
};

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

const QString sVoltageChannelDescription = "Measuring channel 0..250V 50Hz/150kHz";
const QString sCurrentChannelDescription = "Measuring channel 0..1000A 50Hz/150kHz";

Mt310s2SenseInterface::Mt310s2SenseInterface(cSCPI *scpiInterface,
                                             I2cSettings* i2cSettings,
                                             cSenseSettings* senseSettings,
                                             SystemInfo *systemInfo,
                                             AbstractFactoryI2cCtrlPtr ctrlFactory) :
    SenseInterfaceCommon(scpiInterface,
                           i2cSettings,
                           systemInfo,
                           ctrlFactory)
{
    // Init with bad defaults so coder's bugs pop up
    m_nSerialStatus = Adjustment::wrongSNR;

    m_availSenseModesHash["AC"] = modeAC;
    m_availSenseModesHash["HF"] = modeHF;
    m_availSenseModesHash["ADJ"] = modeADJ;

    setChannelAndRanges(senseSettings);
    setSenseMode("AC");
    setNotifierSenseChannelCat(); // only prepared for !!! since we don't have hot plug for measuring channels yet
}

void Mt310s2SenseInterface::setChannelAndRanges(cSenseSettings* senseSettings)
{
    QList<SenseSystem::cChannelSettings*> channelSettings;
    channelSettings = senseSettings->getChannelSettings();

    // for com5003 our sense had 3 voltage and 3 current measuring channels
    // for mt310 we need 4 voltage and 4 current measuring channels
    SenseChannelCommon* pChannel;
    pChannel = new Mt310s2SenseChannel(m_pSCPIInterface, sVoltageChannelDescription,"V", channelSettings.at(0), 0, m_ctrlFactory);
    m_channelList.append(pChannel);
    pChannel = new Mt310s2SenseChannel(m_pSCPIInterface, sVoltageChannelDescription,"V", channelSettings.at(1), 1, m_ctrlFactory);
    m_channelList.append(pChannel);
    pChannel = new Mt310s2SenseChannel(m_pSCPIInterface, sVoltageChannelDescription,"V", channelSettings.at(2), 2, m_ctrlFactory);
    m_channelList.append(pChannel);
    pChannel = new Mt310s2SenseChannel(m_pSCPIInterface, sVoltageChannelDescription,"V", channelSettings.at(6), 6, m_ctrlFactory);
    m_channelList.append(pChannel);

    pChannel = new Mt310s2SenseChannel(m_pSCPIInterface, sCurrentChannelDescription,"A", channelSettings.at(3), 3, m_ctrlFactory);
    m_channelList.append(pChannel);
    pChannel = new Mt310s2SenseChannel(m_pSCPIInterface, sCurrentChannelDescription,"A", channelSettings.at(4), 4, m_ctrlFactory);
    m_channelList.append(pChannel);
    pChannel = new Mt310s2SenseChannel(m_pSCPIInterface, sCurrentChannelDescription,"A", channelSettings.at(5), 5, m_ctrlFactory);
    m_channelList.append(pChannel);
    pChannel = new Mt310s2SenseChannel(m_pSCPIInterface, sCurrentChannelDescription,"A", channelSettings.at(7), 7, m_ctrlFactory);
    m_channelList.append(pChannel);

    QList<SenseRangeCommon*> rngList;

    int i;
    for (i = 0; i < 4; i++) {
        rngList.clear();
        QString rangeName = "250V";
        if(isInvalidAdjDataOrChannelRangeAvail(m_channelList.at(i)->getName(), rangeName))
            rngList.append(new Mt310s2SenseRange(m_pSCPIInterface, rangeName, true, 250.0, 4415057.0, 5518821.0, 0, rangeFlagsDevice(), createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,    "8V", true,   8.0, 3355443.0, 4194304.0, 1, rangeFlagsDevice(), createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface, "100mV", true,   0.1, 4026532.0, 5033165.0, 2, rangeFlagsDevice(), createJustScpiInterfaceWithAtmelPermission()));

        m_channelList.at(i)->setRangeList(rngList);
    }

    for (i = 4; i < 7; i++) {
        rngList.clear();
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,   "10A", true,  10.0, 3197613.0, 3997016.0,  0, rangeFlagsDevice(), createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,    "5A", true,   5.0, 3197613.0, 3997016.0,  1, rangeFlagsDevice(), createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,  "2.5A", true,   2.5, 3997016.0, 4996270.0,  2, rangeFlagsDevice(), createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,  "1.0A", true,   1.0, 4177527.0, 5221909.0,  3, rangeFlagsDevice(), createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface, "500mA", true,   0.5, 4177527.0, 5221909.0,  4, rangeFlagsDevice(), createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface, "250mA", true,  0.25, 4177527.0, 5221909.0,  5, rangeFlagsDevice(), createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface, "100mA", true,   0.1, 4177527.0, 5221909.0,  6, rangeFlagsDevice(), createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,  "50mA", true,  0.05, 4177527.0, 5221909.0,  7, rangeFlagsDevice(), createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,  "25mA", true, 0.025, 4177527.0, 5221909.0,  8, rangeFlagsDevice(), createJustScpiInterfaceWithAtmelPermission()));

        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,    "8V", false,  8.0, 3355443.0, 4194304.0,  9, rangeFlagsIntern(), createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,    "5V", false,  5.0, 4194304.0, 5242880.0, 10, rangeFlagsIntern(), createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,    "2V", false,  2.0, 2835586.0, 3544483.0, 11, rangeFlagsIntern(), createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,    "1V", false,  1.0, 2835586.0, 3544483.0, 12, rangeFlagsIntern(), createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface, "500mV", false,  0.5, 3544482.0, 4430603.0, 13, rangeFlagsIntern(), createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface, "200mV", false,  0.2, 2684355.0, 3355444.0, 14, rangeFlagsIntern(), createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface, "100mV", false,  0.1, 3355443.0, 4194304.0, 15, rangeFlagsIntern(), createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,  "50mV", false, 0.05, 3355443.0, 4194304.0, 16, rangeFlagsIntern(), createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,  "20mV", false, 0.02, 2684355.0, 3355444.0, 17, rangeFlagsIntern(), createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,  "10mV", false, 0.01, 3355443.0, 4194304.0, 18, rangeFlagsIntern(), createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,   "5mV", false,0.005, 3355443.0, 4194304.0, 19, rangeFlagsIntern(), createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,   "2mV", false,0.002, 2684355.0, 3355444.0, 20, rangeFlagsIntern(), createJustScpiInterfaceWithAtmelPermission()));

        m_channelList.at(i)->setRangeList(rngList);
    }

    // IAUX
    rngList.clear();
    // rValue = 1e-12 (yes a pico Ampere error - for 100% range sample value):
    // We did not yet find the place but it seems 0.0 turns into a divisor causing DSP to run almost infinite
    // loop on range change when running power1module on a channnel supportin 0A
    rngList.append(new Mt310s2SenseRange(m_pSCPIInterface, SenseRangeCommon::rangeNameCurrentNull, true,    1e-12, 3197613.0, 3997016.0,  0, rangeFlagsDevice(), createJustScpiInterfaceWithAtmelPermission()));
    rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,    "8V", false,   8.0, 3355443.0, 4194304.0,  9, rangeFlagsIntern(), createJustScpiInterfaceWithAtmelPermission()));
    rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,    "5V", false,   5.0, 4194304.0, 5242880.0, 10, rangeFlagsIntern(), createJustScpiInterfaceWithAtmelPermission()));
    rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,    "2V", false,   2.0, 2835586.0, 3544483.0, 11, rangeFlagsIntern(), createJustScpiInterfaceWithAtmelPermission()));
    rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,    "1V", false,   1.0, 2835586.0, 3544483.0, 12, rangeFlagsIntern(), createJustScpiInterfaceWithAtmelPermission()));
    rngList.append(new Mt310s2SenseRange(m_pSCPIInterface, "500mV", false,   0.5, 3544482.0, 4430603.0, 13, rangeFlagsIntern(), createJustScpiInterfaceWithAtmelPermission()));
    rngList.append(new Mt310s2SenseRange(m_pSCPIInterface, "200mV", false,   0.2, 2684355.0, 3355444.0, 14, rangeFlagsIntern(), createJustScpiInterfaceWithAtmelPermission()));
    rngList.append(new Mt310s2SenseRange(m_pSCPIInterface, "100mV", false,   0.1, 3355443.0, 4194304.0, 15, rangeFlagsIntern(), createJustScpiInterfaceWithAtmelPermission()));
    rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,  "50mV", false,  0.05, 3355443.0, 4194304.0, 16, rangeFlagsIntern(), createJustScpiInterfaceWithAtmelPermission()));
    rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,  "20mV", false,  0.02, 2684355.0, 3355444.0, 17, rangeFlagsIntern(), createJustScpiInterfaceWithAtmelPermission()));
    rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,  "10mV", false,  0.01, 3355443.0, 4194304.0, 18, rangeFlagsIntern(), createJustScpiInterfaceWithAtmelPermission()));
    rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,   "5mV", false, 0.005, 3355443.0, 4194304.0, 19, rangeFlagsIntern(), createJustScpiInterfaceWithAtmelPermission()));
    rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,   "2mV", false, 0.002, 2684355.0, 3355444.0, 20, rangeFlagsIntern(), createJustScpiInterfaceWithAtmelPermission()));

    m_channelList.at(7)->setRangeList(rngList);
}


QString Mt310s2SenseInterface::getPcbName()
{
    return LeiterkartenName;
}

QString Mt310s2SenseInterface::getXmlType()
{
    return "PCBAdjustmentData";
}

bool Mt310s2SenseInterface::isRangePartOfAdjXmlExport(SenseRangeCommon *range)
{
    return range->getMMask() & Direct;
}

int Mt310s2SenseInterface::rangeFlagsDevice()
{
    return modeAC | modeADJ | Direct;
}

int Mt310s2SenseInterface::rangeFlagsIntern()
{
    return modeADJ | Direct;
}

int Mt310s2SenseInterface::rangeFlagsExtern()
{
    return modeAC | Clamp;
}

int Mt310s2SenseInterface::rangeFlagsExternDc()
{
    return rangeFlagsExtern() | modeDC;
}

const char *Mt310s2SenseInterface::getAdjExportedVersion()
{
    return ServerVersion;
}

void Mt310s2SenseInterface::handleScpiReadWriteMMode(cProtonetCommand *protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    if (cmd.isQuery())
        protoCmd->m_sOutput  = m_notifierSenseMMode.getString();
    else {
        if (cmd.isCommand(1)) {
            QString mode = cmd.getParam(0);
            if(setSenseMode(mode))
                protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::ack];
            else
                protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
        }
        else
            protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
    }
    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

QString Mt310s2SenseInterface::scpiReadSenseGroupCatalog(QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return ";"; // this server has no grouping constraints
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

RangeAdjInterface *Mt310s2SenseInterface::createJustScpiInterfaceWithAtmelPermission()
{
    return new RangeAdjInterface(m_pSCPIInterface, AdjustScpiValueFormatterFactory::createMt310s2AdjFormatter());
}

bool Mt310s2SenseInterface::setSenseMode(QString sMode)
{
    if (m_availSenseModesHash.contains(sMode)) {
        quint8 mode = m_availSenseModesHash[sMode];
        m_ctrlFactory->getMModeController()->setMeasMode((mode >> 1) & 1); // set the atmels mode here...atmel only knows ac and hf
        for(auto channel : qAsConst(m_channelList))
            channel->setMMode(mode);
        m_currSenseMode = sMode;
        setNotifierSenseMMode();
        return true;
    }
    return false;
}
