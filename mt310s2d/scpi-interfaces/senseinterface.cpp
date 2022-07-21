#include "senseinterface.h"
#include "mt310s2dglobal.h"
#include "mt310s2d.h"
#include "adjustment.h"
#include "justdata.h"
#include "mt310s2justdata.h"
#include "sensechannel.h"
#include "senserange.h"
#include "systeminfo.h"
#include "adjflash.h"
#include <protonetcommand.h>
#include <scpiconnection.h>
#include "resource.h"
#include "micro-controller-io/atmel.h"
#include "settings/i2csettings.h"
#include "settings/sensesettings.h"
#include <xmlsettings.h>
#include <QList>
#include <QStringList>
#include <QDomElement>
#include <QDomDocument>
#include <QDomText>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <syslog.h>

cSenseInterface::cSenseInterface(cMT310S2dServer *server) :
    cResource(server->getSCPIInterface()),
    cAdjFlash(server->m_pI2CSettings->getDeviceNode(),
              server->m_pI2CSettings->getI2CAdress(i2cSettings::flash),
              server->m_pI2CSettings->createNullMuxer()),
    m_pMyServer(server),
    m_pSystemInfo(server->m_pSystemInfo)
{
    // Init with bad defaults so coder's bugs pop up
    m_nVersionStatus = Adjustment::wrongVERS;
    m_nSerialStatus = Adjustment::wrongSNR;

    m_MModeHash["AC"]   = SenseSystem::modeAC;
    m_MModeHash["HF"]   = SenseSystem::modeHF;
    m_MModeHash["ADJ"]   = SenseSystem::modeADJ;

    QList<SenseSystem::cChannelSettings*> channelSettings;
    channelSettings = server->m_pSenseSettings->getChannelSettings();

    // for com5003 our sense had 3 voltage and 3 current measuring channels
    // for mt310 we need 4 voltage and 4 current measuring channels
    cSenseChannel* pChannel;
    pChannel = new cSenseChannel(m_pSCPIInterface, SenseSystem::sVoltageChannelDescription,"V", channelSettings.at(0), 0);
    m_ChannelList.append(pChannel);
    pChannel = new cSenseChannel(m_pSCPIInterface, SenseSystem::sVoltageChannelDescription,"V", channelSettings.at(1), 1);
    m_ChannelList.append(pChannel);
    pChannel = new cSenseChannel(m_pSCPIInterface, SenseSystem::sVoltageChannelDescription,"V", channelSettings.at(2), 2);
    m_ChannelList.append(pChannel);
    pChannel = new cSenseChannel(m_pSCPIInterface, SenseSystem::sVoltageChannelDescription,"V", channelSettings.at(6), 6);
    m_ChannelList.append(pChannel);

    pChannel = new cSenseChannel(m_pSCPIInterface, SenseSystem::sCurrentChannelDescription,"A", channelSettings.at(3), 3);
    m_ChannelList.append(pChannel);
    pChannel = new cSenseChannel(m_pSCPIInterface, SenseSystem::sCurrentChannelDescription,"A", channelSettings.at(4), 4);
    m_ChannelList.append(pChannel);
    pChannel = new cSenseChannel(m_pSCPIInterface, SenseSystem::sCurrentChannelDescription,"A", channelSettings.at(5), 5);
    m_ChannelList.append(pChannel);
    pChannel = new cSenseChannel(m_pSCPIInterface, SenseSystem::sCurrentChannelDescription,"A", channelSettings.at(7), 7);
    m_ChannelList.append(pChannel);

    QList<cSenseRange*> rngList;

    int i;
    for (i = 0; i < 4; i++) {
        rngList.clear();
        rngList.append(new cSenseRange(m_pSCPIInterface,  "250V",   "250V", true, 250.0, 4415057.0, 5518821.0, 8388607.0, 0, SenseSystem::modeAC | SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));
        rngList.append(new cSenseRange(m_pSCPIInterface,    "8V",     "8V", true,   8.0, 3355443.0, 4194304.0, 8388607.0, 1, SenseSystem::modeAC | SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));
        rngList.append(new cSenseRange(m_pSCPIInterface, "100mV" , "100mV", true,   0.1, 4026532.0, 5033165.0, 8388607.0, 2, SenseSystem::modeAC | SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));

        m_ChannelList.at(i)->setRangeList(rngList);
    }

    for (i = 4; i < 7; i++) {
        rngList.clear();
        rngList.append(new cSenseRange(m_pSCPIInterface,   "10A",   "10A", true,  10.0, 3197613.0, 3997016.0, 8388607.0,  0, SenseSystem::modeAC | SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));
        rngList.append(new cSenseRange(m_pSCPIInterface,    "5A",    "5A", true,   5.0, 3197613.0, 3997016.0, 8388607.0,  1, SenseSystem::modeAC | SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));
        rngList.append(new cSenseRange(m_pSCPIInterface,  "2.5A",  "2.5A", true,   2.5, 3997016.0, 4996270.0, 8388607.0,  2, SenseSystem::modeAC | SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));
        rngList.append(new cSenseRange(m_pSCPIInterface,  "1.0A",  "1.0A", true,   1.0, 4177527.0, 5221909.0, 8388607.0,  3, SenseSystem::modeAC | SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));
        rngList.append(new cSenseRange(m_pSCPIInterface, "500mA", "500mA", true,   0.5, 4177527.0, 5221909.0, 8388607.0,  4, SenseSystem::modeAC | SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));
        rngList.append(new cSenseRange(m_pSCPIInterface, "250mA", "250mA", true,  0.25, 4177527.0, 5221909.0, 8388607.0,  5, SenseSystem::modeAC | SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));
        rngList.append(new cSenseRange(m_pSCPIInterface, "100mA", "100mA", true,   0.1, 4177527.0, 5221909.0, 8388607.0,  6, SenseSystem::modeAC | SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));
        rngList.append(new cSenseRange(m_pSCPIInterface,  "50mA",  "50mA", true,  0.05, 4177527.0, 5221909.0, 8388607.0,  7, SenseSystem::modeAC | SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));
        rngList.append(new cSenseRange(m_pSCPIInterface,  "25mA",  "25mA", true, 0.025, 4177527.0, 5221909.0, 8388607.0,  8, SenseSystem::modeAC | SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));

        rngList.append(new cSenseRange(m_pSCPIInterface,    "8V",    "8V", false,   8.0, 3355443.0, 4194304.0, 8388607.0,  9, SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));
        rngList.append(new cSenseRange(m_pSCPIInterface,    "5V",    "5V", false,   5.0, 4194304.0, 5242880.0, 8388607.0, 10, SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));
        rngList.append(new cSenseRange(m_pSCPIInterface,    "2V",    "2V", false,   2.0, 2835586.0, 3544483.0, 8388607.0, 11, SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));
        rngList.append(new cSenseRange(m_pSCPIInterface,    "1V",    "1V", false,   1.0, 2835586.0, 3544483.0, 8388607.0, 12, SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));
        rngList.append(new cSenseRange(m_pSCPIInterface, "500mV", "500mV", false,   0.5, 3544482.0, 4430603.0, 8388607.0, 13, SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));
        rngList.append(new cSenseRange(m_pSCPIInterface, "200mV", "200mV", false,   0.2, 2684355.0, 3355444.0, 8388607.0, 14, SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));
        rngList.append(new cSenseRange(m_pSCPIInterface, "100mV", "100mV", false,   0.1, 3355443.0, 4194304.0, 8388607.0, 15, SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));
        rngList.append(new cSenseRange(m_pSCPIInterface,  "50mV",  "50mV", false,  0.05, 3355443.0, 4194304.0, 8388607.0, 16, SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));
        rngList.append(new cSenseRange(m_pSCPIInterface,  "20mV",  "20mV", false,  0.02, 2684355.0, 3355444.0, 8388607.0, 17, SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));
        rngList.append(new cSenseRange(m_pSCPIInterface,  "10mV",  "10mV", false,  0.01, 3355443.0, 4194304.0, 8388607.0, 18, SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));
        rngList.append(new cSenseRange(m_pSCPIInterface,   "5mV",   "5mV", false, 0.005, 3355443.0, 4194304.0, 8388607.0, 19, SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));
        rngList.append(new cSenseRange(m_pSCPIInterface,   "2mV",   "2mV", false, 0.002, 2684355.0, 3355444.0, 8388607.0, 20, SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));

        m_ChannelList.at(i)->setRangeList(rngList);
    }

    rngList.clear();
    rngList.append(new cSenseRange(m_pSCPIInterface,    "0A",    "--", true,    0.0, 3197613.0, 3997016.0, 8388607.0,  0, SenseSystem::modeAC | SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));
    rngList.append(new cSenseRange(m_pSCPIInterface,    "8V",    "8V", false,   8.0, 3355443.0, 4194304.0, 8388607.0,  9, SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));
    rngList.append(new cSenseRange(m_pSCPIInterface,    "5V",    "5V", false,   5.0, 4194304.0, 5242880.0, 8388607.0, 10, SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));
    rngList.append(new cSenseRange(m_pSCPIInterface,    "2V",    "2V", false,   2.0, 2835586.0, 3544483.0, 8388607.0, 11, SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));
    rngList.append(new cSenseRange(m_pSCPIInterface,    "1V",    "1V", false,   1.0, 2835586.0, 3544483.0, 8388607.0, 12, SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));
    rngList.append(new cSenseRange(m_pSCPIInterface, "500mV", "500mV", false,   0.5, 3544482.0, 4430603.0, 8388607.0, 13, SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));
    rngList.append(new cSenseRange(m_pSCPIInterface, "200mV", "200mV", false,   0.2, 2684355.0, 3355444.0, 8388607.0, 14, SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));
    rngList.append(new cSenseRange(m_pSCPIInterface, "100mV", "100mV", false,   0.1, 3355443.0, 4194304.0, 8388607.0, 15, SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));
    rngList.append(new cSenseRange(m_pSCPIInterface,  "50mV",  "50mV", false,  0.05, 3355443.0, 4194304.0, 8388607.0, 16, SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));
    rngList.append(new cSenseRange(m_pSCPIInterface,  "20mV",  "20mV", false,  0.02, 2684355.0, 3355444.0, 8388607.0, 17, SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));
    rngList.append(new cSenseRange(m_pSCPIInterface,  "10mV",  "10mV", false,  0.01, 3355443.0, 4194304.0, 8388607.0, 18, SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));
    rngList.append(new cSenseRange(m_pSCPIInterface,   "5mV",   "5mV", false, 0.005, 3355443.0, 4194304.0, 8388607.0, 19, SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));
    rngList.append(new cSenseRange(m_pSCPIInterface,   "2mV",   "2mV", false, 0.002, 2684355.0, 3355444.0, 8388607.0, 20, SenseSystem::modeADJ | SenseSystem::Direct, new cMT310S2JustData(m_pSCPIInterface)));

    m_ChannelList.at(7)->setRangeList(rngList);
    setSenseMode("AC");
    setNotifierSenseChannelCat(); // only prepared for !!! since we don't have hot plug for measuring channels yet
    m_sVersion = SenseSystem::Version;
}

cSenseInterface::~cSenseInterface()
{
    for(auto channel : m_ChannelList) {
        delete channel;
    }
    m_ChannelList.clear();
}

void cSenseInterface::initSCPIConnection(QString leadingNodes)
{
    if (leadingNodes != "") {
        leadingNodes += ":";
    }
    cSCPIDelegate* delegate = new cSCPIDelegate(QString("%1SENSE").arg(leadingNodes),"VERSION",SCPI::isQuery, m_pSCPIInterface, SenseSystem::cmdVersion);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SENSE").arg(leadingNodes),"MMODE",SCPI::isQuery | SCPI::isCmdwP , m_pSCPIInterface, SenseSystem::cmdMMode);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SENSE:MMODE").arg(leadingNodes),"CATALOG",SCPI::isQuery, m_pSCPIInterface, SenseSystem::cmdMModeCat );
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SENSE:CHANNEL").arg(leadingNodes),"CATALOG", SCPI::isQuery, m_pSCPIInterface, SenseSystem::cmdChannelCat);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SENSE:GROUP").arg(leadingNodes),"CATALOG", SCPI::isQuery, m_pSCPIInterface, SenseSystem::cmdGroupCat);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SENSE:CORRECTION").arg(leadingNodes),"INIT", SCPI::isCmd, m_pSCPIInterface, SenseSystem::initAdjData);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SENSE:CORRECTION").arg(leadingNodes),"COMPUTE", SCPI::isCmd, m_pSCPIInterface, SenseSystem::computeAdjData);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    for(auto channel : m_ChannelList) {
        // we also must connect the signals for notification and for output
        connect(channel, &cSCPIConnection::strNotifier, this, &cSCPIConnection::strNotifier);
        connect(channel, SIGNAL(cmdExecutionDone(cProtonetCommand*)), this, SIGNAL(cmdExecutionDone(cProtonetCommand*)));
        channel->initSCPIConnection(QString("%1SENSE").arg(leadingNodes));
    }
    QString cmdParent = QString("STATUS:PCB");
    delegate = new cSCPIDelegate(cmdParent, "ADJUSTMENT", SCPI::isQuery, m_pSCPIInterface, SenseSystem::cmdStatAdjustment);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
}

cSenseChannel *cSenseInterface::getChannel(QString &name)
{
    cSenseChannel *channelFound = nullptr;
    for(auto channel : m_ChannelList) {
        if(channel->getName() == name) {
            channelFound = channel;
            break;
        }
    }
    return channelFound;
}

QString cSenseInterface::getChannelSystemName(quint16 ctrlChannel)
{
    QString nameFound;
    for(auto channel : m_ChannelList) {
        if(channel->getCtrlChannel() == ctrlChannel) {
            nameFound = channel->getName();
            break;
        }
    }
    return nameFound;
}

cSenseRange* cSenseInterface::getRange(QString channelName, QString rangeName)
{
    cSenseRange* rangeFound = nullptr;
    cSenseChannel *channelFound = getChannel(channelName);
    if(channelFound) {
        rangeFound = channelFound->getRange(rangeName);
    }
    return rangeFound;
}

quint8 cSenseInterface::getAdjustmentStatus()
{
    quint8 adjustmentStatusMask = Adjustment::adjusted;
    // Loop adjustment state for all channels
    for(auto channel : m_ChannelList) {
        quint8 channelFlags = channel->getAdjustmentStatus();
        // Currently there is one flag in channel flags only
        if((channelFlags & JustData::Justified)== 0) {
            adjustmentStatusMask = Adjustment::notAdjusted;
            break;
        }
    }
    // if we read wrong serial or version we are not adjusted in any case
    quint8 sernoVersionStatusMask = m_nSerialStatus | m_nVersionStatus;
    if (sernoVersionStatusMask != 0) {
        adjustmentStatusMask = Adjustment::notAdjusted;
        adjustmentStatusMask |= sernoVersionStatusMask;
    }
    return adjustmentStatusMask;
}

void cSenseInterface::executeCommand(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case SenseSystem::cmdVersion:
        protoCmd->m_sOutput = m_ReadVersion(protoCmd->m_sInput);
        if (protoCmd->m_bwithOutput)
            emit cmdExecutionDone(protoCmd);
        break;
    case SenseSystem::cmdMMode:
        m_ReadWriteMMode(protoCmd);
        // we have to start statemachine when setting
        break;
    case SenseSystem::cmdMModeCat:
        protoCmd->m_sOutput = m_ReadMModeCatalog(protoCmd->m_sInput);
        if (protoCmd->m_bwithOutput)
            emit cmdExecutionDone(protoCmd);
        break;
    case SenseSystem::cmdChannelCat:
        protoCmd->m_sOutput = m_ReadSenseChannelCatalog(protoCmd->m_sInput);
        if (protoCmd->m_bwithOutput)
            emit cmdExecutionDone(protoCmd);
        break;
    case SenseSystem::cmdGroupCat:
        protoCmd->m_sOutput = m_ReadSenseGroupCatalog(protoCmd->m_sInput);
        if (protoCmd->m_bwithOutput)
            emit cmdExecutionDone(protoCmd);
        break;
    case SenseSystem::initAdjData:
        protoCmd->m_sOutput = m_InitSenseAdjData(protoCmd->m_sInput);
        if (protoCmd->m_bwithOutput)
            emit cmdExecutionDone(protoCmd);
        break;
    case SenseSystem::computeAdjData:
        protoCmd->m_sOutput = m_ComputeSenseAdjData(protoCmd->m_sInput);
        if (protoCmd->m_bwithOutput)
            emit cmdExecutionDone(protoCmd);
        break;
    case SenseSystem::cmdStatAdjustment:
        protoCmd->m_sOutput = handleScpiReadAdjStatus(protoCmd->m_sInput);
        if (protoCmd->m_bwithOutput)
            emit cmdExecutionDone(protoCmd);
        break;
    }
}

bool cSenseInterface::importAdjData(QDataStream &stream)
{
    char flashdata[200];
    char* s = flashdata;

    stream.skipRawData(6); // we don't need count and chksum
    stream >> s;
    if (QString(s) != "ServerVersion") {
        syslog(LOG_ERR,"flashmemory read, ServerVersion not found\n");
        return false; // unexpected data
    }

    stream >> s;
    QString SVersion = QString(s);
    stream >> s; // we take the device name

    QString sysDevName = m_pSystemInfo->getDeviceName();
    if (QString(s) != sysDevName) {
        syslog(LOG_ERR,"flashmemory read, contains wrong pcb name: flash %s / µC %s\n",
               s, qPrintable(sysDevName));
        return false; // wrong pcb name
    }

    stream >> s; // we take the device version now
    QString qs = QString(s);

    bool enable = false;
    pAtmel->getEEPROMAccessEnable(enable);

    QString sDV = m_pSystemInfo->getDeviceVersion();
    if (qs != sDV) {
        // test ob sich nur die hinteren nummern der lca bzw. ctrl version geändert haben
        // indem die hinteren stellen der nummern aus sDeviceVersion nach s übertragen werden
        // und anschliessend nochmal verglichen wird

        QString ss, sd, ss2, sd2;
        ss = qs.section(';',2,2); // LCA: x.xx
        ss2 = '.' +ss.section('.',1,1); // .xx
        sd = m_pSystemInfo->getDeviceVersion().section(';',2,2); // LCA: x.yy
        sd2 = '.' +sd.section('.',1,1); // .yy
        ss.replace(ss2,sd2); // tausch .xx durch .yy
        qs.replace(qs.section(';',2,2), ss); // LCA: x.yy -> s

        ss = qs.section(';',3,3); // CTRL: x.xx
        ss2 = '.' +ss.section('.',1,1); // .xx
        sd = m_pSystemInfo->getDeviceVersion().section(';',3,3); // CTRL: x.yy
        sd2 = '.' +sd.section('.',1,1); // .yy
        ss.replace(ss2,sd2); // tausch .xx durch .yy
        qs.replace(qs.section(';',3,3), ss); // CTRL: x.yy -> s

        if (qs != sDV) {
            syslog(LOG_ERR,"flashmemory read, contains wrong versionnumber: flash %s / µC %s\n",
                   qPrintable(qs), qPrintable(sDV));
            m_nVersionStatus |= Adjustment::wrongVERS;
            if (!enable) {
                return false; // wrong version number
            }
        }
        else {
            m_nVersionStatus = 0; // ok
        }
    }
    else { // version full match
        m_nVersionStatus = 0; // ok
    }

    stream >> s; // we take the serial number now
    QString sysSerNo = m_pSystemInfo->getSerialNumber();
    if (QString(s) != sysSerNo) {
        syslog(LOG_ERR, "flashmemory read, contains wrong serialnumber flash: %s / µC: %s\n",
               s, qPrintable(sysSerNo));
        m_nSerialStatus |= Adjustment::wrongSNR;
        if (!enable) {
            return false; // wrong serial number
        }
    }
    else {
        m_nSerialStatus = 0; // ok
    }

    stream >> s;
    QDateTime DateTime = QDateTime::fromString(QString(s), Qt::TextDate); // datum und uhrzeit übernehmen
    while (!stream.atEnd()) {
        bool done;
        stream >> s;
        QString  JDataSpecs = s; // Type:Channel:Range

        QStringList spec;
        spec = JDataSpecs.split(':');

        done = false;
        if (spec.at(0) == "SENSE" ) {
            cSenseChannel* chn;
            QString s = spec.at(1);
            if ((chn = getChannel(s)) != nullptr) {
                s = spec.at(2);
                cSenseRange* rng = chn->getRange(s);
                if (rng != nullptr) {
                    rng->getJustData()->Deserialize(stream);
                    done = true;
                }
            }
        }
        if (!done) {
            cMT310S2JustData* dummy; // if we could not find the owner of that data
            dummy = new cMT310S2JustData(m_pSCPIInterface);
            dummy->Deserialize(stream); // we read the data from stream to keep it in flow
            delete dummy;
        }
    }
    return (true);
}

void cSenseInterface::exportAdjData(QDataStream &stream)
{
    // ab version v1.02
    stream << "ServerVersion";
    stream << ServerVersion;
    stream << m_pSystemInfo->getDeviceName().toStdString().c_str(); // leiterkarten name aus atmel gelesen
    stream << m_pSystemInfo->getDeviceVersion().toStdString().c_str(); // geräte name versionsnummern ...
    stream << m_pSystemInfo->getSerialNumber().toStdString().c_str(); // seriennummer
    stream << QDateTime::currentDateTime().toString(Qt::TextDate).toStdString().c_str(); // datum,uhrzeit
    for(auto channel : m_ChannelList) {
        for(auto range : channel->getRangeList()) {
            if ((range->getMMask() & SenseSystem::Direct)> 0)
            {
                QString spec = QString("%1:%2:%3")
                     .arg("SENSE")
                     .arg(channel->getName())
                     .arg(range->getName());

                stream << spec.toLatin1();
                range->getJustData()->Serialize(stream);
            }
        }
    }
}

QString cSenseInterface::exportXMLString(int indent)
{
    QDomDocument justqdom (QStringLiteral("PCBAdjustmentData"));

    QDomElement pcbtag = justqdom.createElement( "PCB" );
    justqdom.appendChild( pcbtag );

    QDomElement tag = justqdom.createElement( "Type" );
    pcbtag.appendChild( tag );
    QDomText t = justqdom.createTextNode(LeiterkartenName);
    tag.appendChild( t );

    tag = justqdom.createElement( "VersionNumber" );
    pcbtag.appendChild( tag );
    t = justqdom.createTextNode(m_pSystemInfo->getDeviceVersion() );
    tag.appendChild( t );

    tag = justqdom.createElement( "SerialNumber" );
    pcbtag.appendChild( tag );
    t = justqdom.createTextNode(m_pSystemInfo->getSerialNumber());
    tag.appendChild( t );

    tag = justqdom.createElement( "Date" );
    pcbtag.appendChild( tag );
    QDateTime currDateTime = QDateTime::currentDateTime();
    QDate d = currDateTime.date();
    t = justqdom.createTextNode(d.toString(Qt::TextDate));
    tag.appendChild( t );

    tag = justqdom.createElement( "Time" );
    pcbtag.appendChild( tag );
    QTime ti = currDateTime.time();
    t = justqdom.createTextNode(ti.toString(Qt::TextDate));
    tag.appendChild( t );

    QDomElement adjtag = justqdom.createElement( "Adjustment" );
    pcbtag.appendChild( adjtag );

    QDomElement chksumtag = justqdom.createElement("Chksum");
    adjtag.appendChild(chksumtag);
    t = justqdom.createTextNode(QString("0x%1").arg(m_nChecksum,0,16));
    chksumtag.appendChild(t);

    QDomElement typeTag = justqdom.createElement( "Sense");
    adjtag.appendChild(typeTag);

    for(auto channel : m_ChannelList) {
        QDomText t;
        QDomElement chtag = justqdom.createElement( "Channel" );
        typeTag.appendChild( chtag );
        QDomElement nametag = justqdom.createElement( "Name" );
        chtag.appendChild(nametag);
        t = justqdom.createTextNode(channel->getName());
        nametag.appendChild( t );

        for(auto range : channel->getRangeList()) {
            if ((range->getMMask() & SenseSystem::Direct)> 0) {
                QDomElement rtag = justqdom.createElement( "Range" );
                chtag.appendChild( rtag );

                nametag = justqdom.createElement( "Name" );
                rtag.appendChild(nametag);

                t = justqdom.createTextNode(range->getName());
                nametag.appendChild( t );

                QDomElement gpotag = justqdom.createElement( "Gain" );
                rtag.appendChild(gpotag);
                QDomElement tag = justqdom.createElement( "Status" );
                QString jdata = range->getJustData()->m_pGainCorrection->SerializeStatus();
                t = justqdom.createTextNode(jdata);
                gpotag.appendChild(tag);
                tag.appendChild(t);
                tag = justqdom.createElement( "Coefficients" );
                gpotag.appendChild(tag);
                jdata = range->getJustData()->m_pGainCorrection->SerializeCoefficients();
                t = justqdom.createTextNode(jdata);
                tag.appendChild(t);
                tag = justqdom.createElement( "Nodes" );
                gpotag.appendChild(tag);
                jdata = range->getJustData()->m_pGainCorrection->SerializeNodes();
                t = justqdom.createTextNode(jdata);
                tag.appendChild(t);

                gpotag = justqdom.createElement( "Phase" );
                rtag.appendChild(gpotag);
                tag = justqdom.createElement( "Status" );
                jdata = range->getJustData()->m_pPhaseCorrection->SerializeStatus();
                t = justqdom.createTextNode(jdata);
                tag.appendChild(t);
                gpotag.appendChild(tag);
                tag = justqdom.createElement( "Coefficients" );
                gpotag.appendChild(tag);
                jdata = range->getJustData()->m_pPhaseCorrection->SerializeCoefficients();
                t = justqdom.createTextNode(jdata);
                tag.appendChild(t);
                tag = justqdom.createElement( "Nodes" );
                gpotag.appendChild(tag);
                jdata = range->getJustData()->m_pPhaseCorrection->SerializeNodes();
                t = justqdom.createTextNode(jdata);
                tag.appendChild(t);

                gpotag = justqdom.createElement( "Offset" );
                rtag.appendChild(gpotag);
                tag = justqdom.createElement( "Status" );
                jdata = range->getJustData()->m_pOffsetCorrection->SerializeStatus();
                t = justqdom.createTextNode(jdata);
                tag.appendChild(t);
                gpotag.appendChild(tag);
                tag = justqdom.createElement( "Coefficients" );
                gpotag.appendChild(tag);
                jdata = range->getJustData()->m_pOffsetCorrection->SerializeCoefficients();
                t = justqdom.createTextNode(jdata);
                tag.appendChild(t);
                tag = justqdom.createElement( "Nodes" );
                gpotag.appendChild(tag);
                jdata = range->getJustData()->m_pOffsetCorrection->SerializeNodes();
                t = justqdom.createTextNode(jdata);
                tag.appendChild(t);
            }
        }
    }
    return justqdom.toString(indent);
}

void cSenseInterface::m_ComputeSenseAdjData()
{
    for(auto channel : m_ChannelList) {
        channel->computeJustData();
    }
}

bool cSenseInterface::importXMLDocument(QDomDocument* qdomdoc) // n steht auf einem element dessen tagname channel ist
{
    QDomDocumentType TheDocType = qdomdoc->doctype ();
    if  (TheDocType.name() != QString("PCBAdjustmentData")) {
        syslog(LOG_ERR,"justdata import, wrong xml documentype\n");
        return false;
    }
    QDomElement rootElem = qdomdoc->documentElement();
    QDomNodeList nl = rootElem.childNodes();
    bool TypeOK = false;
    bool VersionNrOK = false;
    bool SerialNrOK = false;
    bool DateOK = false;
    bool TimeOK = false;
    bool ChksumOK = false;
    bool SenseOK = false;
    for (int i = 0; i < nl.length() ; i++) {
        QDomNode qdNode = nl.item(i);
        QDomElement qdElem = qdNode.toElement();
        if ( qdElem.isNull() ) {
            syslog(LOG_ERR,"justdata import, format error in xml file\n");
            return false;
        }
        QString tName = qdElem.tagName();
        if (tName == "Type") {
            if ( !(TypeOK = (qdElem.text() == QString(LeiterkartenName)))) {
                syslog(LOG_ERR,"justdata import, wrong type information in xml file\n");
                return false;
            }
        }
        else if (tName == "SerialNumber") {
            if (  !(SerialNrOK = (qdElem.text() == m_pSystemInfo->getSerialNumber() )) ) {
               syslog(LOG_ERR,"justdata import, wrong serialnumber in xml file\n");
               return false;
            }
        }
        else if (tName == "VersionNumber") {
           if ( ! ( VersionNrOK= (qdElem.text() == m_pSystemInfo->getDeviceVersion()) ) ) {
               syslog(LOG_ERR,"justdata import, wrong versionnumber in xml file\n");
               return false;
           }
        }
        else if (tName=="Date") {
            QDate d = QDate::fromString(qdElem.text(),Qt::TextDate);
            DateOK = d.isValid();
        }
        else if (tName=="Time") {
            QTime t = QTime::fromString(qdElem.text(),Qt::TextDate);
            TimeOK = t.isValid();
        }
        else if (tName == "Adjustment") {
            if ( VersionNrOK && SerialNrOK && DateOK && TimeOK && TypeOK) {
                QDomNodeList adjChildNl = qdElem.childNodes();
                for (qint32 j = 0; j < adjChildNl.length(); j++) {
                    qdNode = adjChildNl.item(j);
                    QString tagName = qdNode.toElement().tagName();
                    if (tagName == "Chksum") {
                        ChksumOK = true; // we don't read it actually because if something was changed outside ....
                    }
                    else if (qdNode.toElement().tagName() == "Sense") {
                        SenseOK = true;
                        QDomNodeList channelNl = qdNode.childNodes(); // we have a list our channels entries now
                        for (qint32 i = 0; i < channelNl.length(); i++) {
                            QDomNode chnNode = channelNl.item(i); // we iterate over all channels from xml file
                            QDomNodeList chnEntryNl = chnNode.childNodes();
                            cSenseChannel* chnPtr = nullptr;
                            cSenseRange* rngPtr = nullptr;
                            for (qint32 j = 0; j < chnEntryNl.length(); j++) {
                                QString Name;
                                QDomNode ChannelJustNode = chnEntryNl.item(j);
                                qdElem = ChannelJustNode.toElement();
                                QString tName = qdElem.tagName();
                                qDebug() << tName;
                                if (tName == "Name") {
                                    Name = qdElem.text();
                                    qDebug() << Name;
                                    chnPtr = getChannel(Name);
                                }
                                else if (tName == "Range") {
                                    if (chnPtr) { // if we know this channel
                                        QDomNodeList chnJustNl = ChannelJustNode.childNodes();
                                        for (qint32 k = 0; k < chnJustNl.length(); k++) {
                                            QDomNode RangeJustNode = chnJustNl.item(k);
                                            qdElem = RangeJustNode.toElement();
                                            tName = qdElem.tagName();
                                            qDebug() << tName;
                                            if (tName == "Name") {
                                                Name = qdElem.text();
                                                qDebug() << Name;
                                                rngPtr = chnPtr->getRange(Name);
                                            }
                                            cJustData* pJustData = nullptr;
                                            if (rngPtr != nullptr) {
                                                if (tName == "Gain") {
                                                    pJustData = rngPtr->getJustData()->m_pGainCorrection;
                                                }
                                                if (tName == "Phase") {
                                                    pJustData = rngPtr->getJustData()->m_pPhaseCorrection;
                                                }
                                                if (tName == "Offset") {
                                                    pJustData = rngPtr->getJustData()->m_pOffsetCorrection;
                                                }
                                            }
                                            if (pJustData) {
                                                QDomNodeList jdataNl = RangeJustNode.childNodes();
                                                for (qint32 k = 0; k < jdataNl.count(); k++) {
                                                    QDomNode jTypeNode = jdataNl.item(k);
                                                    QString jTypeName = jTypeNode.toElement().tagName();
                                                    QString jdata = jTypeNode.toElement().text();
                                                    if (jTypeName == "Status") {
                                                        pJustData->DeserializeStatus(jdata);
                                                    }
                                                    if (jTypeName == "Coefficients") {
                                                        pJustData->DeserializeCoefficients(jdata);
                                                    }
                                                    if (jTypeName == "Nodes") {
                                                        pJustData->DeserializeNodes(jdata);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else {
                syslog(LOG_ERR,"justdata import, xml file contains strange data\n");
                return false;
            }
        }
        else {
            syslog(LOG_ERR,"justdata import, xml file contains strange data\n");
            return false;
        }
    }
    return ChksumOK && SenseOK;
}

void cSenseInterface::registerResource(cRMConnection *rmConnection, quint16 port)
{
    msgNrList.clear();
    for(auto channel : m_ChannelList) {
        register1Resource(rmConnection, m_pMyServer->getMsgNr(), QString("SENSE;%1;1;%2;%3;")
                         .arg(channel->getName())
                         .arg(channel->getDescription())
                         .arg(port));
    }
    // additional we register measuring mode switch as resource
    register1Resource(rmConnection, m_pMyServer->getMsgNr(), QString("SENSE;MMODE;1;%1;%2;")
                      .arg(SenseSystem::sMeasuringModeDescription)
                      .arg(port));

}

void cSenseInterface::unregisterResource(cRMConnection *rmConnection)
{
    msgNrList.clear();
    for(auto channel : m_ChannelList) {
        unregister1Resource(rmConnection, m_pMyServer->getMsgNr(), QString("SENSE;%1;")
                         .arg(channel->getName()));
    }
}

QString cSenseInterface::m_ReadVersion(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        return m_sVersion;
    }
    else {
        return SCPI::scpiAnswer[SCPI::nak];
    }
}

void cSenseInterface::m_ReadWriteMMode(cProtonetCommand *protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    if (cmd.isQuery()) {
        emit strNotifier(&notifierSenseMMode);
        protoCmd->m_sOutput  = notifierSenseMMode.getString();
        if (protoCmd->m_bwithOutput) {
            emit cmdExecutionDone(protoCmd);
        }
    }
    else {
        if (cmd.isCommand(1)) {
            QString mode = cmd.getParam(0);
            if (setSenseMode(mode)) {
                protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::ack];
            }
            else {
                protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];
            }
        }
        else {
            protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];
        }
        if (protoCmd->m_bwithOutput) {
            emit cmdExecutionDone(protoCmd);
        }
    }
}

QString cSenseInterface::m_ReadMModeCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        return m_MModeHash.keys().join(";");
    }
    else {
        return SCPI::scpiAnswer[SCPI::nak];
    }
}

QString cSenseInterface::m_ReadSenseChannelCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        emit strNotifier(&notifierSenseChannelCat);
        return notifierSenseChannelCat.getString();
    }
    else {
        return SCPI::scpiAnswer[SCPI::nak];
    }
}

QString cSenseInterface::m_ReadSenseGroupCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        QString s = ";"; // this server has no grouping constraints
        return s;
    }
    else {
        return SCPI::scpiAnswer[SCPI::nak];
    }
}

QString cSenseInterface::m_InitSenseAdjData(QString &sInput)
{
    cSCPICommand cmd = sInput;
    // cmd.isCommand(0) is not correct but we leave it for compatibility
    if ( cmd.isCommand(0) || (cmd.isCommand(1) && (cmd.getParam(0) == ""))) {
        for(auto channel : m_ChannelList) {
            channel->initJustData();
        }
        return SCPI::scpiAnswer[SCPI::ack];
    }
    else {
        return SCPI::scpiAnswer[SCPI::nak];
    }
}

QString cSenseInterface::m_ComputeSenseAdjData(QString& sInput)
{
    cSCPICommand cmd = sInput;
    // cmd.isCommand(0) is not correct but we leave it for compatibility
    if ( cmd.isCommand(0) || (cmd.isCommand(1) && (cmd.getParam(0) == ""))) {
        m_ComputeSenseAdjData();
        return SCPI::scpiAnswer[SCPI::ack];
    }
    else {
        return SCPI::scpiAnswer[SCPI::nak];
    }
}

QString cSenseInterface::handleScpiReadAdjStatus(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        return  QString("%1").arg(getAdjustmentStatus());
    }
    else {
        return SCPI::scpiAnswer[SCPI::nak];
    }
}

void cSenseInterface::setNotifierSenseMMode()
{
    notifierSenseMMode = m_sMMode;
}

void cSenseInterface::setNotifierSenseChannelCat()
{
    // only prepared for !!! since we don't have hot plug for measuring channels yet
    int i;
    QString s;
    for (i = 0; i < m_ChannelList.count()-1; i++ ) {
        s += m_ChannelList.at(i)->getName() + ";";
    }
    s += m_ChannelList.at(i)->getName();
    notifierSenseChannelCat = s;
}

bool cSenseInterface::setSenseMode(QString sMode)
{
    bool ret = false;
    if (m_MModeHash.contains(sMode)) {
        quint8 mode;
        mode = m_MModeHash[sMode];
        pAtmel->setMeasMode((mode >> 1) & 1); // set the atmels mode here...atmel only knows ac and hf
        for(auto channel : m_ChannelList) {
            channel->setMMode(mode);
        }
        m_sMMode = sMode;
        setNotifierSenseMMode();
        ret = true;
    }
    return ret;
}
