#include "mt310s2senseinterface.h"
#include "mt310s2dglobal.h"
#include "justdatainterface.h"
#include "adjustmentflags.h"
#include "mt310s2sensechannel.h"
#include "mt310s2senserange.h"
#include "adjustmenteeprom.h"
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

    m_availSenseModesHash["AC"] = SenseSystem::modeAC;
    m_availSenseModesHash["HF"] = SenseSystem::modeHF;
    m_availSenseModesHash["ADJ"] = SenseSystem::modeADJ;

    QList<SenseSystem::cChannelSettings*> channelSettings;
    channelSettings = senseSettings->getChannelSettings();

    // for com5003 our sense had 3 voltage and 3 current measuring channels
    // for mt310 we need 4 voltage and 4 current measuring channels
    SenseChannelCommon* pChannel;
    pChannel = new Mt310s2SenseChannel(m_pSCPIInterface, SenseSystem::sVoltageChannelDescription,"V", channelSettings.at(0), 0, m_ctrlFactory);
    m_channelList.append(pChannel);
    pChannel = new Mt310s2SenseChannel(m_pSCPIInterface, SenseSystem::sVoltageChannelDescription,"V", channelSettings.at(1), 1, m_ctrlFactory);
    m_channelList.append(pChannel);
    pChannel = new Mt310s2SenseChannel(m_pSCPIInterface, SenseSystem::sVoltageChannelDescription,"V", channelSettings.at(2), 2, m_ctrlFactory);
    m_channelList.append(pChannel);
    pChannel = new Mt310s2SenseChannel(m_pSCPIInterface, SenseSystem::sVoltageChannelDescription,"V", channelSettings.at(6), 6, m_ctrlFactory);
    m_channelList.append(pChannel);

    pChannel = new Mt310s2SenseChannel(m_pSCPIInterface, SenseSystem::sCurrentChannelDescription,"A", channelSettings.at(3), 3, m_ctrlFactory);
    m_channelList.append(pChannel);
    pChannel = new Mt310s2SenseChannel(m_pSCPIInterface, SenseSystem::sCurrentChannelDescription,"A", channelSettings.at(4), 4, m_ctrlFactory);
    m_channelList.append(pChannel);
    pChannel = new Mt310s2SenseChannel(m_pSCPIInterface, SenseSystem::sCurrentChannelDescription,"A", channelSettings.at(5), 5, m_ctrlFactory);
    m_channelList.append(pChannel);
    pChannel = new Mt310s2SenseChannel(m_pSCPIInterface, SenseSystem::sCurrentChannelDescription,"A", channelSettings.at(7), 7, m_ctrlFactory);
    m_channelList.append(pChannel);

    QList<SenseRangeCommon*> rngList;

    int i;
    for (i = 0; i < 4; i++) {
        rngList.clear();
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,  "250V", true, 250.0, 4415057.0, 5518821.0, 0, SenseSystem::modeAC | SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,    "8V", true,   8.0, 3355443.0, 4194304.0, 1, SenseSystem::modeAC | SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface, "100mV", true,   0.1, 4026532.0, 5033165.0, 2, SenseSystem::modeAC | SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));

        m_channelList.at(i)->setRangeList(rngList);
    }

    for (i = 4; i < 7; i++) {
        rngList.clear();
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,   "10A", true,  10.0, 3197613.0, 3997016.0,  0, SenseSystem::modeAC | SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,    "5A", true,   5.0, 3197613.0, 3997016.0,  1, SenseSystem::modeAC | SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,  "2.5A", true,   2.5, 3997016.0, 4996270.0,  2, SenseSystem::modeAC | SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,  "1.0A", true,   1.0, 4177527.0, 5221909.0,  3, SenseSystem::modeAC | SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface, "500mA", true,   0.5, 4177527.0, 5221909.0,  4, SenseSystem::modeAC | SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface, "250mA", true,  0.25, 4177527.0, 5221909.0,  5, SenseSystem::modeAC | SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface, "100mA", true,   0.1, 4177527.0, 5221909.0,  6, SenseSystem::modeAC | SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,  "50mA", true,  0.05, 4177527.0, 5221909.0,  7, SenseSystem::modeAC | SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,  "25mA", true, 0.025, 4177527.0, 5221909.0,  8, SenseSystem::modeAC | SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));

        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,    "8V", false,  8.0, 3355443.0, 4194304.0,  9, SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,    "5V", false,  5.0, 4194304.0, 5242880.0, 10, SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,    "2V", false,  2.0, 2835586.0, 3544483.0, 11, SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,    "1V", false,  1.0, 2835586.0, 3544483.0, 12, SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface, "500mV", false,  0.5, 3544482.0, 4430603.0, 13, SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface, "200mV", false,  0.2, 2684355.0, 3355444.0, 14, SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface, "100mV", false,  0.1, 3355443.0, 4194304.0, 15, SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,  "50mV", false, 0.05, 3355443.0, 4194304.0, 16, SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,  "20mV", false, 0.02, 2684355.0, 3355444.0, 17, SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,  "10mV", false, 0.01, 3355443.0, 4194304.0, 18, SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,   "5mV", false,0.005, 3355443.0, 4194304.0, 19, SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));
        rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,   "2mV", false,0.002, 2684355.0, 3355444.0, 20, SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));

        m_channelList.at(i)->setRangeList(rngList);
    }

    // IAUX
    rngList.clear();
    // rValue = 1e-12 (yes a pico Ampere error - for 100% range sample value):
    // We did not yet find the place but it seems 0.0 turns into a divisor causing DSP to run almost infinite
    // loop on range change when running power1module on a channnel supportin 0A
    rngList.append(new Mt310s2SenseRange(m_pSCPIInterface, SenseRangeCommon::rangeNameCurrentNull, true,    1e-12, 3197613.0, 3997016.0,  0, SenseSystem::modeAC | SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));
    rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,    "8V", false,   8.0, 3355443.0, 4194304.0,  9, SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));
    rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,    "5V", false,   5.0, 4194304.0, 5242880.0, 10, SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));
    rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,    "2V", false,   2.0, 2835586.0, 3544483.0, 11, SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));
    rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,    "1V", false,   1.0, 2835586.0, 3544483.0, 12, SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));
    rngList.append(new Mt310s2SenseRange(m_pSCPIInterface, "500mV", false,   0.5, 3544482.0, 4430603.0, 13, SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));
    rngList.append(new Mt310s2SenseRange(m_pSCPIInterface, "200mV", false,   0.2, 2684355.0, 3355444.0, 14, SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));
    rngList.append(new Mt310s2SenseRange(m_pSCPIInterface, "100mV", false,   0.1, 3355443.0, 4194304.0, 15, SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));
    rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,  "50mV", false,  0.05, 3355443.0, 4194304.0, 16, SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));
    rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,  "20mV", false,  0.02, 2684355.0, 3355444.0, 17, SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));
    rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,  "10mV", false,  0.01, 3355443.0, 4194304.0, 18, SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));
    rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,   "5mV", false, 0.005, 3355443.0, 4194304.0, 19, SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));
    rngList.append(new Mt310s2SenseRange(m_pSCPIInterface,   "2mV", false, 0.002, 2684355.0, 3355444.0, 20, SenseSystem::modeADJ | SenseSystem::Direct, createJustScpiInterfaceWithAtmelPermission()));

    m_channelList.at(7)->setRangeList(rngList);
    setSenseMode("AC");
    setNotifierSenseChannelCat(); // only prepared for !!! since we don't have hot plug for measuring channels yet
}

void Mt310s2SenseInterface::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1SENSE").arg(leadingNodes),"VERSION",SCPI::isQuery, m_pSCPIInterface, SenseSystem::cmdVersion);
    addDelegate(QString("%1SENSE").arg(leadingNodes),"MMODE",SCPI::isQuery | SCPI::isCmdwP , m_pSCPIInterface, SenseSystem::cmdMMode, &m_notifierSenseMMode);
    addDelegate(QString("%1SENSE:MMODE").arg(leadingNodes),"CATALOG",SCPI::isQuery, m_pSCPIInterface, SenseSystem::cmdMModeCat);
    addDelegate(QString("%1SENSE:CHANNEL").arg(leadingNodes),"CATALOG", SCPI::isQuery, m_pSCPIInterface, SenseSystem::cmdChannelCat, &m_notifierSenseChannelCat);
    addDelegate(QString("%1SENSE:GROUP").arg(leadingNodes),"CATALOG", SCPI::isQuery, m_pSCPIInterface, SenseSystem::cmdGroupCat);
    addDelegate(QString("%1SENSE:CORRECTION").arg(leadingNodes),"INIT", SCPI::isCmd, m_pSCPIInterface, SenseSystem::initAdjData);
    addDelegate(QString("%1SENSE:CORRECTION").arg(leadingNodes),"COMPUTE", SCPI::isCmd, m_pSCPIInterface, SenseSystem::computeAdjData);
    for(auto channel : qAsConst(m_channelList)) {
        // we also must connect the signals for notification and for output
        connect(channel, &ScpiConnection::sendNotification, this, &ScpiConnection::sendNotification);
        connect(channel, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
        connect(this, &ScpiConnection::removingSubscribers, channel, &ScpiConnection::onRemoveSubscribers);
        channel->initSCPIConnection(QString("%1SENSE").arg(leadingNodes));
    }
    QString cmdParent = QString("STATUS:PCB");
    addDelegate(cmdParent, "ADJUSTMENT", SCPI::isQuery, m_pSCPIInterface, SenseSystem::cmdStatAdjustment);
}

void Mt310s2SenseInterface::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case SenseSystem::cmdVersion:
        protoCmd->m_sOutput = scpiReadVersion(protoCmd->m_sInput);
        if (protoCmd->m_bwithOutput)
            emit cmdExecutionDone(protoCmd);
        break;
    case SenseSystem::cmdMMode:
        scpiReadWriteMMode(protoCmd);
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
        protoCmd->m_sOutput = scpiReadSenseGroupCatalog(protoCmd->m_sInput);
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
        protoCmd->m_sOutput = scpiReadAdjStatus(protoCmd->m_sInput);
        if (protoCmd->m_bwithOutput)
            emit cmdExecutionDone(protoCmd);
        break;
    }
}

bool Mt310s2SenseInterface::importAdjData(QDataStream &stream)
{
    char flashdata[200];
    char* s = flashdata;

    stream.skipRawData(6); // we don't need count and chksum
    stream >> s;
    if (QString(s) != "ServerVersion") {
        qCritical("Flashmemory read: ServerVersion not found");
        return false;
    }

    stream >> s; // version: not checked anymore
    stream >> s; // we take the device name

    QString sysDevName = m_systemInfo->getDeviceName();
    if (QString(s) != sysDevName) {
        qCritical("Flashmemory read: Wrong device name: flash %s / µC %s",
                  s, qPrintable(sysDevName));
        return false;
    }

    stream >> s; // we take the device version now

    bool enable = false;
    m_ctrlFactory->getPermissionCheckController()->hasPermission(enable);

    stream >> s; // we take the serial number now
    QString sysSerNo = m_systemInfo->getSerialNumber();
    if (QString(s) != sysSerNo) {
        qCritical("Flashmemory read, contains wrong serialnumber flash: %s / µC: %s",
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
            SenseChannelCommon* chn;
            QString s = spec.at(1);
            if ((chn = getChannel(s)) != nullptr) {
                s = spec.at(2);
                SenseRangeCommon* rng = chn->getRange(s);
                if (rng != nullptr) {
                    rng->getJustData()->Deserialize(stream);
                    done = true;
                }
            }
        }
        if (!done) {
            // owner of data read not found: read dummy to keep serialization in sync
            RangeAdjInterface* dummy = createJustScpiInterfaceWithAtmelPermission();
            dummy->Deserialize(stream);
            delete dummy;
        }
    }
    return (true);
}

void Mt310s2SenseInterface::exportAdjData(QDataStream &stream, QDateTime dateTimeWrite)
{
    // ab version v1.02
    stream << "ServerVersion";
    stream << ServerVersion;
    stream << m_systemInfo->getDeviceName().toStdString().c_str(); // leiterkarten name aus atmel gelesen
    stream << m_systemInfo->getDeviceVersion().toStdString().c_str(); // geräte name versionsnummern ...
    stream << m_systemInfo->getSerialNumber().toStdString().c_str(); // seriennummer
    stream << dateTimeWrite.toString(Qt::TextDate).toStdString().c_str(); // datum,uhrzeit
    for(auto channel : qAsConst(m_channelList)) {
        for(auto range : channel->getRangeList()) {
            if ((range->getMMask() & SenseSystem::Direct)> 0) {
                QString spec = QString("%1:%2:%3")
                     .arg("SENSE")
                     .arg(channel->getName())
                     .arg(range->getRangeName());

                stream << spec.toLatin1();
                range->getJustData()->Serialize(stream);
            }
        }
    }
}

QString Mt310s2SenseInterface::exportXMLString(int indent)
{
    QDomDocument justqdom (QStringLiteral("PCBAdjustmentData"));

    QDomElement pcbtag = justqdom.createElement("PCB");
    justqdom.appendChild( pcbtag );

    QDomElement tag = justqdom.createElement( "Type" );
    pcbtag.appendChild( tag );
    QDomText t = justqdom.createTextNode(LeiterkartenName);
    tag.appendChild( t );

    tag = justqdom.createElement("VersionNumber");
    pcbtag.appendChild( tag );
    t = justqdom.createTextNode(m_systemInfo->getDeviceVersion() );
    tag.appendChild( t );

    tag = justqdom.createElement("SerialNumber");
    pcbtag.appendChild( tag );
    t = justqdom.createTextNode(m_systemInfo->getSerialNumber());
    tag.appendChild( t );

    tag = justqdom.createElement("Date");
    pcbtag.appendChild( tag );
    QDateTime currDateTime = QDateTime::currentDateTime();
    QDate d = currDateTime.date();
    t = justqdom.createTextNode(d.toString(Qt::TextDate));
    tag.appendChild( t );

    tag = justqdom.createElement("Time");
    pcbtag.appendChild( tag );
    QTime ti = currDateTime.time();
    t = justqdom.createTextNode(ti.toString(Qt::TextDate));
    tag.appendChild( t );

    QDomElement adjtag = justqdom.createElement("Adjustment");
    pcbtag.appendChild( adjtag );

    QDomElement chksumtag = justqdom.createElement("Chksum");
    adjtag.appendChild(chksumtag);
    t = justqdom.createTextNode(QString("0x%1").arg(getChecksum(), 0, 16));
    chksumtag.appendChild(t);

    QDomElement typeTag = justqdom.createElement("Sense");
    adjtag.appendChild(typeTag);

    for(auto channel : qAsConst(m_channelList)) {
        QDomText t;
        QDomElement chtag = justqdom.createElement("Channel");
        typeTag.appendChild( chtag );
        QDomElement nametag = justqdom.createElement("Name");
        chtag.appendChild(nametag);
        t = justqdom.createTextNode(channel->getName());
        nametag.appendChild( t );

        for(auto range : qAsConst(channel->getRangeList())) {
            if ((range->getMMask() & SenseSystem::Direct)> 0) {
                QDomElement rtag = justqdom.createElement("Range");
                chtag.appendChild( rtag );

                nametag = justqdom.createElement("Name");
                rtag.appendChild(nametag);

                t = justqdom.createTextNode(range->getRangeName());
                nametag.appendChild( t );

                QDomElement gpotag;
                const QStringList listAdjTypes = QStringList() << "Gain" << "Phase" << "Offset";
                for(const auto &adjType : listAdjTypes) {
                    gpotag = justqdom.createElement(adjType);
                    rtag.appendChild(gpotag);
                    JustDataInterface* adjDataInterface = range->getJustData()->getAdjInterface(adjType);
                    QDomElement tag = justqdom.createElement("Status");
                    QString jdata = adjDataInterface->SerializeStatus();
                    t = justqdom.createTextNode(jdata);
                    gpotag.appendChild(tag);
                    tag.appendChild(t);
                    tag = justqdom.createElement("Coefficients");
                    gpotag.appendChild(tag);
                    jdata = adjDataInterface->SerializeCoefficients();
                    t = justqdom.createTextNode(jdata);
                    tag.appendChild(t);
                    tag = justqdom.createElement("Nodes");
                    gpotag.appendChild(tag);
                    jdata = adjDataInterface->SerializeNodes();
                    t = justqdom.createTextNode(jdata);
                    tag.appendChild(t);
                }
            }
        }
    }
    return justqdom.toString(indent);
}

bool Mt310s2SenseInterface::importXMLDocument(QDomDocument* qdomdoc) // n steht auf einem element dessen tagname channel ist
{
    QDomDocumentType TheDocType = qdomdoc->doctype ();
    if  (TheDocType.name() != QString("PCBAdjustmentData")) {
        qCritical("Justdata import: wrong xml documentype");
        return false;
    }
    QDomElement rootElem = qdomdoc->documentElement();
    QDomNodeList nl = rootElem.childNodes();
    bool TypeOK = false;
    bool SerialNrOK = false;
    bool DateOK = false;
    bool TimeOK = false;
    bool ChksumOK = false;
    bool SenseOK = false;
    for (int i = 0; i < nl.length() ; i++) {
        QDomNode qdNode = nl.item(i);
        QDomElement qdElem = qdNode.toElement();
        if ( qdElem.isNull() ) {
            qCritical("Justdata import: Format error in xml file");
            return false;
        }
        QString tName = qdElem.tagName();
        if (tName == "Type") {
            if ( !(TypeOK = (qdElem.text() == QString(LeiterkartenName)))) {
                qCritical("Justdata import: Wrong type information");
                return false;
            }
        }
        else if (tName == "SerialNumber") {
            SerialNrOK = qdElem.text() == m_systemInfo->getSerialNumber();
            if (!SerialNrOK) {
                qCritical("Justdata import, Wrong serialnumber");
                return false;
            }
        }
        else if (tName == "VersionNumber")
            continue;
        else if (tName=="Date") {
            QDate d = QDate::fromString(qdElem.text(),Qt::TextDate);
            DateOK = d.isValid();
        }
        else if (tName=="Time") {
            QTime t = QTime::fromString(qdElem.text(),Qt::TextDate);
            TimeOK = t.isValid();
        }
        else if (tName == "Adjustment") {
            if ( SerialNrOK && DateOK && TimeOK && TypeOK) {
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
                            SenseChannelCommon* chnPtr = nullptr;
                            SenseRangeCommon* rngPtr = nullptr;
                            for (qint32 j = 0; j < chnEntryNl.length(); j++) {
                                QString Name;
                                QDomNode ChannelJustNode = chnEntryNl.item(j);
                                qdElem = ChannelJustNode.toElement();
                                QString tName = qdElem.tagName();
                                if (tName == "Name") {
                                    Name = qdElem.text();
                                    chnPtr = getChannel(Name);
                                }
                                else if (tName == "Range") {
                                    if (chnPtr) { // if we know this channel
                                        QDomNodeList chnJustNl = ChannelJustNode.childNodes();
                                        for (qint32 k = 0; k < chnJustNl.length(); k++) {
                                            QDomNode RangeJustNode = chnJustNl.item(k);
                                            qdElem = RangeJustNode.toElement();
                                            tName = qdElem.tagName();
                                            if (tName == "Name") {
                                                Name = qdElem.text();
                                                rngPtr = chnPtr->getRange(Name);
                                            }
                                            JustDataInterface* pJustData = nullptr;
                                            if (rngPtr != nullptr)
                                                pJustData = rngPtr->getJustData()->getAdjInterface(tName);
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
                qCritical("Justdata import: xml contains strange data");
                return false;
            }
        }
        else {
            qCritical("Justdata import: xml contains unknown tag '%s'", qPrintable(tName));
            return false;
        }
    }
    return ChksumOK && SenseOK;
}

void Mt310s2SenseInterface::scpiReadWriteMMode(cProtonetCommand *protoCmd)
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

QString Mt310s2SenseInterface::m_InitSenseAdjData(QString &sInput)
{
    cSCPICommand cmd = sInput;
    // cmd.isCommand(0) is not correct but we leave it for compatibility
    if ( cmd.isCommand(0) || (cmd.isCommand(1) && (cmd.getParam(0) == ""))) {
        for(auto channel : qAsConst(m_channelList))
            channel->initJustData();
        return ZSCPI::scpiAnswer[ZSCPI::ack];
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString Mt310s2SenseInterface::m_ComputeSenseAdjData(QString& sInput)
{
    cSCPICommand cmd = sInput;
    // cmd.isCommand(0) is not correct but we leave it for compatibility
    if ( cmd.isCommand(0) || (cmd.isCommand(1) && (cmd.getParam(0) == ""))) {
        computeSenseAdjData();
        return ZSCPI::scpiAnswer[ZSCPI::ack];
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString Mt310s2SenseInterface::scpiReadAdjStatus(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return  QString("%1").arg(getAdjustmentStatus());
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
