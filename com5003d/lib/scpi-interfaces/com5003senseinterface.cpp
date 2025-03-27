#include "com5003senseinterface.h"
#include "com5003dglobal.h"
#include "adjrangescpi.h"
#include "notzeronumgen.h"
#include "com5003sensechannel.h"
#include "com5003senserange.h"
#include "protonetcommand.h"
#include "sensesettings.h"
#include <i2cmultiplexerfactory.h>
#include <xmlsettings.h>
#include "zscpi_response_definitions.h"
#include <QList>
#include <QStringList>
#include <QDomElement>
#include <QDomDocument>
#include <QDomText>
#include <QDebug>

const QString sVoltageChannelDescription = "Measuring channel 0..480V AC";
const QString sCurrentChannelDescription = "Measuring channel 0..160A AC";
const QString sReferenceChannelDescription = "Reference channel 0..10V DC";

Com5003SenseInterface::Com5003SenseInterface(std::shared_ptr<cSCPI> scpiInterface,
                                             I2cSettings *i2cSettings,
                                             RMConnection* rmConnection,
                                             EthSettings *ethSettings,
                                             cSenseSettings *senseSettings, SystemInfo *systemInfo,
                                             AbstractFactoryI2cCtrlPtr ctrlFactory) :
    SenseInterfaceCommon(scpiInterface,
                           i2cSettings,
                           systemInfo,
                           ctrlFactory,
                           QHash<QString, int>{{"AC", modeAC}, {"REF", modeREF}}),
    m_rmConnection(rmConnection),
    m_ethSettings(ethSettings)
{
    m_channelList = setChannelAndRanges(senseSettings,
                                        m_adjData,
                                        m_scpiInterface,
                                        m_ctrlFactory);
    injectAdjToChannelRanges();
    setSenseMode("AC");
    setNotifierSenseChannelCat(); // only prepared for !!! since we don't have hot plug for measuring channels yet

    // we set up our statemachine for changing sense mode
    // we must use a statemachine because we have to synchronize sending of notifier
    // otherwise moduls using this notifier will crash because resources are not registered properly

    m_UnregisterSenseState.addTransition(this, &Com5003SenseInterface::unregisterRdy, &m_RegisterSenseState);
    m_RegisterSenseState.addTransition(this, &Com5003SenseInterface::registerRdy, &m_NotifySenseState);
    m_ChangeSenseModeMachine.addState(&m_UnregisterSenseState);
    m_ChangeSenseModeMachine.addState(&m_RegisterSenseState);
    m_ChangeSenseModeMachine.addState(&m_NotifySenseState);
    m_ChangeSenseModeMachine.setInitialState(&m_UnregisterSenseState);
    connect(&m_UnregisterSenseState, &QAbstractState::entered, this, &Com5003SenseInterface::unregisterSense);
    connect(&m_RegisterSenseState, &QAbstractState::entered, this, &Com5003SenseInterface::registerSense);
    connect(&m_NotifySenseState, &QAbstractState::entered, this, &Com5003SenseInterface::notifySense);
}

QList<SenseChannelCommon*> Com5003SenseInterface::setChannelAndRanges(cSenseSettings *senseSettings,
                                                                       AdjDataPtr adjData,
                                                                       std::shared_ptr<cSCPI> scpi,
                                                                       AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    constexpr int rangeFlagsDevice = modeAC;
    constexpr int rangeFlagsReference = modeREF;
    Q_UNUSED(adjData) // not use in for same signature as mt310s2
    //constexpr int rangeFlagsIntern = 0; // not used yet no clamp plugs

    QList<SenseSystem::cChannelSettings*> channelSettings = senseSettings->getChannelSettings();
    QList<SenseChannelCommon*> channels;

    // default our sense has 3 voltage and 3 current measuring channels
    SenseChannelCommon* pChannel;
    pChannel = new Com5003SenseChannel(scpi, sVoltageChannelDescription,"V", channelSettings.at(0), 0, ctrlFactory);
    channels.append(pChannel);
    pChannel = new Com5003SenseChannel(scpi, sVoltageChannelDescription,"V", channelSettings.at(1), 1, ctrlFactory);
    channels.append(pChannel);
    pChannel = new Com5003SenseChannel(scpi, sVoltageChannelDescription,"V", channelSettings.at(2), 2, ctrlFactory);
    channels.append(pChannel);
    pChannel = new Com5003SenseChannel(scpi, sCurrentChannelDescription,"A", channelSettings.at(3), 3, ctrlFactory);
    channels.append(pChannel);
    pChannel = new Com5003SenseChannel(scpi, sCurrentChannelDescription,"A", channelSettings.at(4), 4, ctrlFactory);
    channels.append(pChannel);
    pChannel = new Com5003SenseChannel(scpi, sCurrentChannelDescription,"A", channelSettings.at(5), 5, ctrlFactory);
    channels.append(pChannel);

    QList<SenseRangeCommon*> rngList;

    int i;
    for (i = 0; i < 3; i++)
    {
        rngList.clear();
        rngList.append(new Com5003SenseRange(scpi,  "480V", true, 480.0, 4712563.0, 5890704.0, 0, rangeFlagsDevice));
        rngList.append(new Com5003SenseRange(scpi,  "240V", true, 240.0, 4712563.0, 5890704.0, 1, rangeFlagsDevice));
        rngList.append(new Com5003SenseRange(scpi,  "120V", true, 120.0, 4712563.0, 5890704.0, 2, rangeFlagsDevice));
        rngList.append(new Com5003SenseRange(scpi,   "60V", true,  60.0, 4712563.0, 5890704.0, 3, rangeFlagsDevice));

        rngList.append(new Com5003SenseRange(scpi,   "12V", true,  12.0, 3887864.8, 4859831.0, 4, rangeFlagsDevice));
        rngList.append(new Com5003SenseRange(scpi,    "5V", true,   5.0, 4516206.0, 5645258.0, 5, rangeFlagsDevice));

        rngList.append(new Com5003SenseRange(scpi,   "R0V", false,  9.0, 3839668.2, 5332873.0, 14, rangeFlagsReference));
        rngList.append(new Com5003SenseRange(scpi,  "R10V", false, 10.0, 4266298.0, 5332873.0, 15, rangeFlagsReference));
        channels.at(i)->setRangeList(rngList);
    }

    for (i = 3; i < 6; i++)
    {
        rngList.clear();
        rngList.append(new Com5003SenseRange(scpi,  "200A", true, 200.0, 6257236.0, 5256077.0, 0, rangeFlagsDevice)); // ???
        rngList.append(new Com5003SenseRange(scpi,  "100A", true, 100.0, 4692928.0, 5866160.0, 1, rangeFlagsDevice));
        rngList.append(new Com5003SenseRange(scpi,   "50A", true,  50.0, 4692928.0, 5866160.0, 2, rangeFlagsDevice));
        rngList.append(new Com5003SenseRange(scpi,   "25A", true, 25.0 , 4692928.0, 5866160.0, 3, rangeFlagsDevice));
        rngList.append(new Com5003SenseRange(scpi,   "10A", true, 10.0 , 4692928.0, 5866160.0, 4, rangeFlagsDevice));
        rngList.append(new Com5003SenseRange(scpi,    "5A", true,  5.0 , 4692928.0, 5866160.0, 5, rangeFlagsDevice));

        rngList.append(new Com5003SenseRange(scpi,  "2.5A", true, 2.5  , 4692928.0, 5866160.0, 6, rangeFlagsDevice));
        rngList.append(new Com5003SenseRange(scpi,  "1.0A", true, 1.0  , 4692928.0, 5866160.0, 7, rangeFlagsDevice));
        rngList.append(new Com5003SenseRange(scpi, "500mA", true, 0.5  , 4692928.0, 5866160.0, 8, rangeFlagsDevice));
        rngList.append(new Com5003SenseRange(scpi, "250mA", true, 0.25 , 4692928.0, 5866160.0, 9, rangeFlagsDevice));
        rngList.append(new Com5003SenseRange(scpi, "100mA", true, 0.1  , 4692928.0, 5866160.0, 10, rangeFlagsDevice));
        rngList.append(new Com5003SenseRange(scpi,  "50mA", true, 0.05 , 4692928.0, 5866160.0, 11, rangeFlagsDevice));
        rngList.append(new Com5003SenseRange(scpi,  "25mA", true, 0.025, 4692928.0, 5866160.0, 12, rangeFlagsDevice));
        rngList.append(new Com5003SenseRange(scpi,  "10mA", true, 0.01 , 4692928.0, 5866160.0, 13, rangeFlagsDevice));
        rngList.append(new Com5003SenseRange(scpi,   "5mA", true, 0.005, 4692928.0, 5866160.0, 14, rangeFlagsDevice));

        rngList.append(new Com5003SenseRange(scpi,   "R0V", false,  9.0, 3839668.2, 5332873.0, 15, rangeFlagsReference));
        rngList.append(new Com5003SenseRange(scpi,  "R10V", false, 10.0, 4266298.0, 5332873.0, 16, rangeFlagsReference));

        channels.at(i)->setRangeList(rngList);
    }
    return channels;
}

int Com5003SenseInterface::rangeFlagsExtern()
{
    // not used yet no clamps supported
    return 0;
}

int Com5003SenseInterface::rangeFlagsExternDc()
{
    // not used yet no clamps supported
    return 0;
}

Com5003SenseInterface::SetModeModeResult Com5003SenseInterface::setSenseModeCom5003Style(QString mode)
{
    if(mode == m_currSenseMode)
        return done;
    if(!m_availSenseModesHash.contains(mode))
        return failed;
    m_currSenseMode = mode;
    return start_statemachine_required;
}

bool Com5003SenseInterface::setSenseMode(QString modeStr)
{
    if (m_availSenseModesHash.contains(modeStr)) {
        m_currSenseMode = modeStr;
        quint8 mode = m_availSenseModesHash[modeStr];
        for(auto channel : qAsConst(m_channelList))
            channel->setMMode(mode);
        changeSense();
        setNotifierSenseMMode();
        return true;
    }
    return false;
}

void Com5003SenseInterface::handleScpiReadWriteMMode(cProtonetCommand *protoCmd)
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
            /*switch(setSenseModeCom5003Style(mode)) {
            case done:
                protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::ack];
                break;
            case failed:
                protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
                break;
            case start_statemachine_required:
                protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::ack];
                sensemodeProtonetCmdList.append(protoCmd);
                m_ChangeSenseModeMachine.start();
                return; // state machine responds
            }*/
        }
        else
            protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
    }
    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

QString Com5003SenseInterface::scpiReadSenseGroupCatalog(QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery()) {
        if (m_availSenseModesHash[m_currSenseMode] == modeAC)
            return ";";
        else
            return "m0,m1,m2;m3,m4,m5;";
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

AdjRangeScpi *Com5003SenseInterface::createJustScpiInterfaceWithAtmelPermission()
{
    return new AdjRangeScpi(m_scpiInterface, AdjustScpiValueFormatterFactory::createCom5003AdjFormatter());
}

void Com5003SenseInterface::unregisterSense()
{
    SenseChannelCommon* pChannel;
    for (int i = 0; i < 6; i++) {
        pChannel = m_channelList.at(i);
        unregister1Resource(m_rmConnection, NotZeroNumGen::getMsgNr(), QString("SENSE;%1;")
                                                                         .arg(pChannel->getName()));
    }
}

void Com5003SenseInterface::changeSense()
{
    if (m_availSenseModesHash[m_currSenseMode] == modeAC) {
        m_channelList.at(0)->setDescription(sVoltageChannelDescription);
        m_channelList.at(0)->setUnit("V");
        m_channelList.at(1)->setDescription(sVoltageChannelDescription);
        m_channelList.at(1)->setUnit("V");
        m_channelList.at(2)->setDescription(sVoltageChannelDescription);
        m_channelList.at(2)->setUnit("V");

        m_channelList.at(3)->setDescription(sCurrentChannelDescription);
        m_channelList.at(3)->setUnit("A");
        m_channelList.at(4)->setDescription(sCurrentChannelDescription);
        m_channelList.at(4)->setUnit("A");
        m_channelList.at(5)->setDescription(sCurrentChannelDescription);
        m_channelList.at(5)->setUnit("A");
        m_ctrlFactory->getMModeController()->setMeasMode(0);
    }
    else { // REF
        for (qint32 i = 0; i < m_channelList.count(); i++) { // for each channel
            m_channelList.at(i)->setDescription(sReferenceChannelDescription);
            m_channelList.at(i)->setUnit("V");
        }
        // correct reference mode (1/'R0V' / 2/'R10V') are set in
        // Com5003SenseChannel::scpiReadWriteRange since the modes are treated
        // like ranges (that's why range groupung is mandatory in REF session)
        m_ctrlFactory->getMModeController()->setMeasMode(1);
    }
}

void Com5003SenseInterface::registerSense()
{
    changeSense();
    registerResource(m_rmConnection, m_ethSettings->getPort(EthSettings::protobufserver));
}

void Com5003SenseInterface::notifySense()
{
    setNotifierSenseMMode(); // we set the notifier synchron after all resources are registered again
    cProtonetCommand *protoCmd;
    protoCmd = sensemodeProtonetCmdList.takeFirst();
    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

const char *Com5003SenseInterface::getAdjExportedVersion()
{
    return ServerVersion;
}

QString Com5003SenseInterface::getPcbName()
{
    return LeiterkartenName;
}

QString Com5003SenseInterface::getXmlType()
{
    return QString("%1AdjustmentData").arg(getPcbName());
}

bool Com5003SenseInterface::isRangePartOfAdjXmlExport(SenseRangeCommon *range)
{
    Q_UNUSED(range)
    // Once COM supports clamps, we have to revisit
    return true;
}
