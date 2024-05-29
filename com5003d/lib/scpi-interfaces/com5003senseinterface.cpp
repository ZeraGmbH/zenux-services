#include "com5003senseinterface.h"
#include "adjflags.h"
#include "com5003dglobal.h"
#include "adjrangeinterface.h"
#include "scpiconnection.h"
#include "resource.h"
#include "notzeronumgen.h"
#include "adjdataiteminterface.h"
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

namespace SenseSystem
{
    enum MMode {
        modeAC,
        modeREF,
    };
    const QString sVoltageChannelDescription = "Measuring channel 0..480V AC";
    const QString sCurrentChannelDescription = "Measuring channel 0..160A AC";
    const QString sReferenceChannelDescription = "Reference channel 0..10V DC";
}

Com5003SenseInterface::Com5003SenseInterface(cSCPI *scpiInterface,
                                             I2cSettings *i2cSettings,
                                             RMConnection* rmConnection,
                                             EthSettings *ethSettings,
                                             cSenseSettings *senseSettings, SystemInfo *systemInfo,
                                             AbstractFactoryI2cCtrlPtr ctrlFactory) :
    SenseInterfaceCommon(scpiInterface,
                           i2cSettings,
                           systemInfo,
                           ctrlFactory),
    m_rmConnection(rmConnection),
    m_ethSettings(ethSettings)
{
    m_currSenseMode = "AC";
    m_availSenseModesHash["AC"] = SenseSystem::modeAC;
    m_availSenseModesHash["REF"] = SenseSystem::modeREF;

    m_ctrlFactory->getMModeController()->setMeasMode(SenseSystem::modeAC); // set the atmels mode too
    setNotifierSenseMMode();

    setChannelAndRanges(senseSettings);

    setNotifierSenseChannelCat(); // only prepared for !!! since we don't have hot plug for measuring channels yet

    // we set up our statemachine for changing sense mode
    // we must use a statemachine because we have to synchronize sending of notifier
    // otherwise moduls using this notifier will crash because resources are not registered properly

    m_UnregisterSenseState.addTransition(this, &Com5003SenseInterface::registerRdy, &m_RegisterSenseState);
    m_RegisterSenseState.addTransition(this, &Com5003SenseInterface::registerRdy, &m_NotifySenseState);
    m_ChangeSenseModeMachine.addState(&m_UnregisterSenseState);
    m_ChangeSenseModeMachine.addState(&m_RegisterSenseState);
    m_ChangeSenseModeMachine.addState(&m_NotifySenseState);
    m_ChangeSenseModeMachine.setInitialState(&m_UnregisterSenseState);
    connect(&m_UnregisterSenseState, &QAbstractState::entered, this, &Com5003SenseInterface::unregisterSense);
    connect(&m_RegisterSenseState, &QAbstractState::entered, this, &Com5003SenseInterface::registerSense);
    connect(&m_NotifySenseState, &QAbstractState::entered, this, &Com5003SenseInterface::notifySense);
}

void Com5003SenseInterface::setChannelAndRanges(cSenseSettings *senseSettings)
{
    QList<SenseSystem::cChannelSettings*> channelSettings;
    channelSettings = senseSettings->getChannelSettings();

    // default our sense has 3 voltage and 3 current measuring channels
    SenseChannelCommon* pChannel;
    pChannel = new Com5003SenseChannel(m_pSCPIInterface, SenseSystem::sVoltageChannelDescription,"V", channelSettings.at(0), 0, m_ctrlFactory);
    m_channelList.append(pChannel);
    pChannel = new Com5003SenseChannel(m_pSCPIInterface, SenseSystem::sVoltageChannelDescription,"V", channelSettings.at(1), 1, m_ctrlFactory);
    m_channelList.append(pChannel);
    pChannel = new Com5003SenseChannel(m_pSCPIInterface, SenseSystem::sVoltageChannelDescription,"V", channelSettings.at(2), 2, m_ctrlFactory);
    m_channelList.append(pChannel);
    pChannel = new Com5003SenseChannel(m_pSCPIInterface, SenseSystem::sCurrentChannelDescription,"A", channelSettings.at(3), 3, m_ctrlFactory);
    m_channelList.append(pChannel);
    pChannel = new Com5003SenseChannel(m_pSCPIInterface, SenseSystem::sCurrentChannelDescription,"A", channelSettings.at(4), 4, m_ctrlFactory);
    m_channelList.append(pChannel);
    pChannel = new Com5003SenseChannel(m_pSCPIInterface, SenseSystem::sCurrentChannelDescription,"A", channelSettings.at(5), 5, m_ctrlFactory);
    m_channelList.append(pChannel);

    QList<SenseRangeCommon*> rngList;

    int i;
    for (i = 0; i < 3; i++)
    {
        rngList.clear();
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,  "480V", true, 480.0, 4712563.0, 5890704.0, 0));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,  "240V", true, 240.0, 4712563.0, 5890704.0, 1));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,  "120V", true, 120.0, 4712563.0, 5890704.0, 2));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,   "60V", true,  60.0, 4712563.0, 5890704.0, 3));

        rngList.append(new Com5003SenseRange(m_pSCPIInterface,   "12V", true,  12.0, 3887864.8, 4859831.0, 4));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,    "5V", true,   5.0, 4516206.0, 5645258.0, 5));

        rngList.append(new Com5003SenseRange(m_pSCPIInterface,   "R0V", false,  9.0, 3839668.2, 5332873.0, 14));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,  "R10V", false, 10.0, 4266298.0, 5332873.0, 15));
        m_channelList.at(i)->setRangeList(rngList);
    }

    for (i = 3; i < 6; i++)
    {
        rngList.clear();
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,  "200A", true, 200.0, 6257236.0, 5256077.0, 0 ));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,  "100A", true, 100.0, 4692928.0, 5866160.0, 1 ));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,   "50A", true,  50.0, 4692928.0, 5866160.0, 2 ));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,   "25A", true, 25.0 , 4692928.0, 5866160.0, 3 ));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,   "10A", true, 10.0 , 4692928.0, 5866160.0, 4 ));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,    "5A", true,  5.0 , 4692928.0, 5866160.0, 5 ));

        rngList.append(new Com5003SenseRange(m_pSCPIInterface,  "2.5A", true, 2.5  , 4692928.0, 5866160.0, 6 ));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,  "1.0A", true, 1.0  , 4692928.0, 5866160.0, 7 ));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface, "500mA", true, 0.5  , 4692928.0, 5866160.0, 8 ));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface, "250mA", true, 0.25 , 4692928.0, 5866160.0, 9 ));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface, "100mA", true, 0.1  , 4692928.0, 5866160.0, 10 ));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,  "50mA", true, 0.05 , 4692928.0, 5866160.0, 11));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,  "25mA", true, 0.025, 4692928.0, 5866160.0, 12));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,  "10mA", true, 0.01 , 4692928.0, 5866160.0, 13));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,   "5mA", true, 0.005, 4692928.0, 5866160.0, 14));

        rngList.append(new Com5003SenseRange(m_pSCPIInterface,   "R0V", false,  9.0, 3839668.2, 5332873.0, 15));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,  "R10V", false, 10.0, 4266298.0, 5332873.0, 16));

        m_channelList.at(i)->setRangeList(rngList);
    }
    injectAdjToChannelRanges();
}

int Com5003SenseInterface::rangeFlagsDevice()
{
    // not used yet Com5003SenseChannel ctor adapts
    return 0;
}

int Com5003SenseInterface::rangeFlagsIntern()
{
    // not used yet no clamp plugs
    return 0;
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

Com5003SenseInterface::SetModeModeResult Com5003SenseInterface::setSenseMode(QString mode)
{
    if(mode == m_currSenseMode)
        return done;
    if(!m_availSenseModesHash.contains(mode))
        return failed;
    m_currSenseMode = mode;
    return start_statemachine_required;
}

void Com5003SenseInterface::handleScpiReadWriteMMode(cProtonetCommand *protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    if (cmd.isQuery())
        protoCmd->m_sOutput  = m_notifierSenseMMode.getString();
    else {
        if (cmd.isCommand(1)) {
            QString mode = cmd.getParam(0);
            switch(setSenseMode(mode)) {
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
            }
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
        QString s;
        if (m_availSenseModesHash[m_currSenseMode] == SenseSystem::modeAC)
            s = ";";
        else
            s = "m0,m1,m2;m3,m4,m5;";
        return s;
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

AdjRangeInterface *Com5003SenseInterface::createJustScpiInterfaceWithAtmelPermission()
{
    return new AdjRangeInterface(m_pSCPIInterface, AdjustScpiValueFormatterFactory::createCom5003AdjFormatter());
}

void Com5003SenseInterface::unregisterSense()
{
    SenseChannelCommon* pChannel;
    m_msgNrList.clear();
    for (int i = 0; i < 6; i++) {
        pChannel = m_channelList.at(i);
        unregister1Resource(m_rmConnection, NotZeroNumGen::getMsgNr(), QString("SENSE;%1;")
                                                                         .arg(pChannel->getName()));
    }
}

void Com5003SenseInterface::registerSense()
{
    QString s;
    // first we change the channels units and descriptions
    if (m_availSenseModesHash[m_currSenseMode] == SenseSystem::modeAC) {
        m_channelList.at(0)->setDescription(SenseSystem::sVoltageChannelDescription);
        m_channelList.at(0)->setUnit(s = "V");
        m_channelList.at(1)->setDescription(SenseSystem::sVoltageChannelDescription);
        m_channelList.at(1)->setUnit(s = "V");
        m_channelList.at(2)->setDescription(SenseSystem::sVoltageChannelDescription);
        m_channelList.at(2)->setUnit(s = "V");

        m_channelList.at(3)->setDescription(SenseSystem::sCurrentChannelDescription);
        m_channelList.at(3)->setUnit(s = "A");
        m_channelList.at(4)->setDescription(SenseSystem::sCurrentChannelDescription);
        m_channelList.at(4)->setUnit(s = "A");
        m_channelList.at(5)->setDescription(SenseSystem::sCurrentChannelDescription);
        m_channelList.at(5)->setUnit(s = "A");
    }
    else {
        for (qint32 i = 0; i < m_channelList.count(); i++) // for each channel
        {
            m_channelList.at(i)->setDescription(SenseSystem::sReferenceChannelDescription);
            m_channelList.at(i)->setUnit(s = "V");
        }
    }

    int dirtyModeNum = m_availSenseModesHash[m_currSenseMode];
    m_ctrlFactory->getMModeController()->setMeasMode(dirtyModeNum); // set the atmels mode too

    // here we do the rest of reconfiguring
    for (qint32 i = 0; i < m_channelList.count(); i++) {
        m_channelList.at(i)->setMMode(dirtyModeNum); // this indirectly changes the channnels alias
        QList<SenseRangeCommon*> list = m_channelList.at(i)->getRangeList();
        for (int j = 0; j < list.count(); j++ )
            list.at(j)->setAvail( !list.at(j)->getAvail()); // we only toggle the ranges avail

    }
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
