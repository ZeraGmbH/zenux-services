#include "com5003senseinterface.h"
#include "adjeepromtools.h"
#include "com5003dglobal.h"
#include "rangeadjinterface.h"
#include "scpiconnection.h"
#include "resource.h"
#include "notzeronumgen.h"
#include "justdatainterface.h"
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

Com5003SenseInterface::Com5003SenseInterface(cSCPI *scpiInterface,
                                             I2cSettings *i2cSettings,
                                             RMConnection* rmConnection,
                                             EthSettings *ethSettings,
                                             cSenseSettings *senseSettings, SystemInfo *systemInfo,
                                             AbstractFactoryI2cCtrlPtr ctrlFactory) :
    cResource(scpiInterface),
    AdjustmentEeprom(i2cSettings->getDeviceNode(),
                     i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress),
                     I2cMultiplexerFactory::createNullMuxer()),
    m_systemInfo(systemInfo),
    m_ctrlFactory(ctrlFactory),
    m_rmConnection(rmConnection),
    m_ethSettings(ethSettings)
{
    // Init with bad defaults so coder's bugs pop up
    m_nSerialStatus = Adjustment::wrongSNR;

    m_nMMode = SenseSystem::modeAC; // default ac measurement
    m_ctrlFactory->getMModeController()->setMeasMode(m_nMMode); // set the atmels mode too
    setNotifierSenseMMode();

    QList<SenseSystem::cChannelSettings*> channelSettings;
    channelSettings = senseSettings->getChannelSettings();

    // default our sense has 3 voltage and 3 current measuring channels
    Com5003SenseChannel* pChannel;
    pChannel = new Com5003SenseChannel(m_pSCPIInterface, SenseSystem::sVoltageChannelDescription,"V", channelSettings.at(0), 0, m_ctrlFactory);
    m_ChannelList.append(pChannel);
    pChannel = new Com5003SenseChannel(m_pSCPIInterface, SenseSystem::sVoltageChannelDescription,"V", channelSettings.at(1), 1, m_ctrlFactory);
    m_ChannelList.append(pChannel);
    pChannel = new Com5003SenseChannel(m_pSCPIInterface, SenseSystem::sVoltageChannelDescription,"V", channelSettings.at(2), 2, m_ctrlFactory);
    m_ChannelList.append(pChannel);
    pChannel = new Com5003SenseChannel(m_pSCPIInterface, SenseSystem::sCurrentChannelDescription,"A", channelSettings.at(3), 3, m_ctrlFactory);
    m_ChannelList.append(pChannel);
    pChannel = new Com5003SenseChannel(m_pSCPIInterface, SenseSystem::sCurrentChannelDescription,"A", channelSettings.at(4), 4, m_ctrlFactory);
    m_ChannelList.append(pChannel);
    pChannel = new Com5003SenseChannel(m_pSCPIInterface, SenseSystem::sCurrentChannelDescription,"A", channelSettings.at(5), 5, m_ctrlFactory);
    m_ChannelList.append(pChannel);

    QList<Com5003SenseRange*> rngList;

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
        m_ChannelList.at(i)->setRangeList(rngList);
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

        m_ChannelList.at(i)->setRangeList(rngList);
    }

    setNotifierSenseChannelCat(); // only prepared for !!! since we don't have hot plug for measuring channels yet
    m_sVersion = SenseSystem::Version;

    // we set up our statemachine for changing sense mode
    // we must use a statemachine because we have to synchronize sending of notifier
    // otherwise moduls using this notifier will crash because resources are not registered properly

    m_UnregisterSenseState.addTransition(this, SIGNAL(registerRdy()), &m_RegisterSenseState);
    m_RegisterSenseState.addTransition(this, SIGNAL(registerRdy()), &m_NotifySenseState);
    m_ChangeSenseModeMachine.addState(&m_UnregisterSenseState);
    m_ChangeSenseModeMachine.addState(&m_RegisterSenseState);
    m_ChangeSenseModeMachine.addState(&m_NotifySenseState);
    m_ChangeSenseModeMachine.setInitialState(&m_UnregisterSenseState);
    connect(&m_UnregisterSenseState, &QAbstractState::entered, this, &Com5003SenseInterface::unregisterSense);
    connect(&m_RegisterSenseState, &QAbstractState::entered, this, &Com5003SenseInterface::registerSense);
    connect(&m_NotifySenseState, &QAbstractState::entered, this, &Com5003SenseInterface::notifySense);
}


Com5003SenseInterface::~Com5003SenseInterface()
{
    int i;
    Com5003SenseChannel* cptr;

    for (i = 0; i < m_ChannelList.count(); i++)
    {
        cptr = m_ChannelList.at(i);
        delete cptr;
    }
}


void Com5003SenseInterface::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1SENSE").arg(leadingNodes),"VERSION",SCPI::isQuery, m_pSCPIInterface, SenseSystem::cmdVersion);
    addDelegate(QString("%1SENSE").arg(leadingNodes),"MMODE",SCPI::isQuery | SCPI::isCmdwP , m_pSCPIInterface, SenseSystem::cmdMMode, &notifierSenseMMode);
    addDelegate(QString("%1SENSE:MMODE").arg(leadingNodes),"CATALOG",SCPI::isQuery, m_pSCPIInterface, SenseSystem::cmdMModeCat);
    addDelegate(QString("%1SENSE:CHANNEL").arg(leadingNodes),"CATALOG", SCPI::isQuery, m_pSCPIInterface, SenseSystem::cmdChannelCat, &notifierSenseChannelCat);
    addDelegate(QString("%1SENSE:GROUP").arg(leadingNodes),"CATALOG", SCPI::isQuery, m_pSCPIInterface, SenseSystem::cmdGroupCat);
    addDelegate(QString("%1SENSE:CORRECTION").arg(leadingNodes),"INIT", SCPI::isCmd, m_pSCPIInterface, SenseSystem::initAdjData);
    addDelegate(QString("%1SENSE:CORRECTION").arg(leadingNodes),"COMPUTE", SCPI::isCmd, m_pSCPIInterface, SenseSystem::computeAdjData);
    for(auto channel : qAsConst(m_ChannelList)) {
        // we also must connect the signals for notification and for output
        connect(channel, &ScpiConnection::sendNotification, this, &ScpiConnection::sendNotification);
        connect(channel, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
        connect(this, &ScpiConnection::removingSubscribers, channel, &ScpiConnection::onRemoveSubscribers);
        channel->initSCPIConnection(QString("%1SENSE").arg(leadingNodes));
    }
}


Com5003SenseChannel *Com5003SenseInterface::getChannel(QString &name)
{
    int i;

    for (i = 0; i < m_ChannelList.count(); i++)
        if (m_ChannelList.at(i)->getName() == name)
            break;

    if (i < m_ChannelList.count())
        return m_ChannelList.at(i);
    else
        return 0;
}


quint8 Com5003SenseInterface::getAdjustmentStatus()
{
    quint8 adjustmentStatusMask = Adjustment::adjusted;
    // Loop adjustment state for all channels
    for(auto channel : qAsConst(m_ChannelList)) {
        quint8 channelFlags = channel->getAdjustmentStatus80Mask();
        // Currently there is one flag in channel flags only
        if((channelFlags & JustDataInterface::Justified)== 0) {
            adjustmentStatusMask = Adjustment::notAdjusted;
            break;
        }
    }
    // if we read wrong serial or version we are not adjusted in any case
    quint8 sernoVersionStatusMask = m_nSerialStatus;
    if (sernoVersionStatusMask != 0) {
        adjustmentStatusMask = Adjustment::notAdjusted;
        adjustmentStatusMask |= sernoVersionStatusMask;
    }
    return adjustmentStatusMask;
}


void Com5003SenseInterface::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
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
        break;
    }
}


bool Com5003SenseInterface::importAdjData(QDataStream &stream)
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
        qCritical("Flashmemory read: Wrong pcb name: flash %s / µC %s",
                  s, qPrintable(sysDevName));
        return false;
    }

    stream >> s; // we take the device version now

    bool enable = false;
    m_ctrlFactory->getPermissionCheckController()->hasPermission(enable);

    stream >> s; // we take the serial number now
    QString sysSerNo = m_systemInfo->getSerialNumber();
    if (QString(s) != sysSerNo) {
        qCritical("Flashmemory read, contains wrong serialnumber: flash %s / µC: %s",
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
            Com5003SenseChannel* chn;
            QString s = spec.at(1);
            if ((chn = getChannel(s)) != nullptr) {
                s = spec.at(2);
                Com5003SenseRange* rng = chn->getRange(s);
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

QString Com5003SenseInterface::exportXMLString(int indent)
{
    QDomDocument justqdom(QString("%1AdjustmentData").arg(LeiterkartenName));

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

    for(auto channel : qAsConst(m_ChannelList)) {
        QDomText t;
        QDomElement chtag = justqdom.createElement("Channel");
        typeTag.appendChild( chtag );
        QDomElement nametag = justqdom.createElement("Name");
        chtag.appendChild(nametag);
        t = justqdom.createTextNode(channel->getName());
        nametag.appendChild( t );

        for(auto range : qAsConst(channel->getRangeList())) {
            // This was stolen from MT and that just stores direct ranges (no clamp ranges)
            // Once COM supports clamps, we have to revisit
            if (true) {
                QDomElement rtag = justqdom.createElement("Range");
                chtag.appendChild( rtag );

                nametag = justqdom.createElement("Name");
                rtag.appendChild(nametag);

                t = justqdom.createTextNode(range->getName());
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


void Com5003SenseInterface::exportAdjData(QDataStream &stream, QDateTime dateTimeWrite)
{
    // ab version v1.02
    stream << "ServerVersion";
    stream << ServerVersion;
    stream << m_systemInfo->getDeviceName().toStdString().c_str(); // leiterkarten name aus atmel gelesen
    stream << m_systemInfo->getDeviceVersion().toStdString().c_str(); // geräte name versionsnummern ...
    stream << m_systemInfo->getSerialNumber().toStdString().c_str(); // seriennummer
    stream << dateTimeWrite.toString(Qt::TextDate).toStdString().c_str(); // datum,uhrzeit
    for(auto channel : qAsConst(m_ChannelList)) {
        for(auto range : channel->getRangeList()) {
            // This was stolen from MT and that just stores direct ranges (no clamp ranges)
            // Once COM supports clamps, we have to revisit
            if (true) {
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


void Com5003SenseInterface::registerResource(RMConnection *rmConnection, quint16 port)
{
    Com5003SenseChannel* pChannel;
    msgNrList.clear();
    for (int i = 0; i < 6; i++)
    {
        pChannel = m_ChannelList.at(i);
        register1Resource(rmConnection, NotZeroNumGen::getMsgNr(), QString("SENSE;%1;1;%2;%3;")
                         .arg(pChannel->getName())
                         .arg(pChannel->getDescription())
                         .arg(port));
    }

    // additional we register measuring mode switch as resource
    register1Resource(rmConnection, NotZeroNumGen::getMsgNr(), QString("SENSE;MMODE;1;%1;%2;")
                      .arg(SenseSystem::sMeasuringModeDescription)
                      .arg(port));

}

void Com5003SenseInterface::computeSenseAdjData()
{
    for(auto channel : qAsConst(m_ChannelList)) {
        channel->computeJustData();
    }
}

bool Com5003SenseInterface::importXMLDocument(QDomDocument *qdomdoc)
{
    QDomDocumentType TheDocType = qdomdoc->doctype ();
    if  (TheDocType.name() != QString("%1AdjustmentData").arg(LeiterkartenName)) {
        qCritical("Justdata import, wrong xml documentype");
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
                            Com5003SenseChannel* chnPtr = nullptr;
                            Com5003SenseRange* rngPtr = nullptr;
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
    return ChksumOK && SenseOK;}


QString Com5003SenseInterface::scpiReadVersion(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return m_sVersion;
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


void Com5003SenseInterface::scpiReadWriteMMode(cProtonetCommand *protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    if (cmd.isQuery()) {
        //return SenseSystem::sMMode[m_nMMode];
        protoCmd->m_sOutput  = notifierSenseMMode.getString();
        if (protoCmd->m_bwithOutput)
            emit cmdExecutionDone(protoCmd);
    }
    else {
        if (cmd.isCommand(1)) {
            QString mode = cmd.getParam(0);
            quint8 oldMode = m_nMMode;
            if (mode == SenseSystem::sMMode[SenseSystem::modeAC] ) {
                protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::ack];
                m_nMMode = SenseSystem::modeAC;
                if (oldMode != m_nMMode) {
                    sensemodeProtonetCmdList.append(protoCmd);
                    m_ChangeSenseModeMachine.start(); // we emit cmdExecutionDone when statemachine is finished
                }
                else
                    emit cmdExecutionDone(protoCmd);
            }
            else if (mode == SenseSystem::sMMode[SenseSystem::modeREF] ) {
                protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::ack];
                m_nMMode = SenseSystem::modeREF;
                if (oldMode != m_nMMode) {
                    sensemodeProtonetCmdList.append(protoCmd);
                    m_ChangeSenseModeMachine.start();
                }
                else
                    emit cmdExecutionDone(protoCmd);
            }
            else {
                protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
                if (protoCmd->m_bwithOutput)
                    emit cmdExecutionDone(protoCmd);
            }
        }
        else {
            protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
            if (protoCmd->m_bwithOutput)
                emit cmdExecutionDone(protoCmd);
        }
    }
}

QString Com5003SenseInterface::m_ReadMModeCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        QString s;
        int i;
        for (i = 0; i < SenseSystem::modeAnz-1; i++ )
            s += SenseSystem::sMMode[i] + ";";
        s += SenseSystem::sMMode[i];
        return s;
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString Com5003SenseInterface::m_ReadSenseChannelCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return notifierSenseChannelCat.getString();
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString Com5003SenseInterface::m_ReadSenseGroupCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        QString s;
        if ( m_nMMode == SenseSystem::modeAC )
            s = ";";
        else
            s = "m0,m1,m2;m3,m4,m5;";
        return s;
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString Com5003SenseInterface::m_InitSenseAdjData(QString &sInput)
{
    cSCPICommand cmd = sInput;
    // cmd.isCommand(0) is not correct but we leave it for compatibility
    if ( cmd.isCommand(0) || (cmd.isCommand(1) && (cmd.getParam(0) == ""))) {
        bool enable;
        if (m_ctrlFactory->getPermissionCheckController()->hasPermission(enable)) {
            if (enable) {
                for (int i = 0; i < m_ChannelList.count(); i++)
                    m_ChannelList.at(i)->initJustData();

                return ZSCPI::scpiAnswer[ZSCPI::ack];
            }
            else
                return ZSCPI::scpiAnswer[ZSCPI::erraut];
        }
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString Com5003SenseInterface::m_ComputeSenseAdjData(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if ( cmd.isCommand(1) && (cmd.getParam(0) == "") ) {
        bool enable;
        if (m_ctrlFactory->getPermissionCheckController()->hasPermission(enable)) {
            if (enable) {
                for (int i = 0; i < m_ChannelList.count(); i++)
                    m_ChannelList.at(i)->computeJustData();
                return ZSCPI::scpiAnswer[ZSCPI::ack];
            }
            else
               return ZSCPI::scpiAnswer[ZSCPI::erraut];
        }
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

RangeAdjInterface *Com5003SenseInterface::createJustScpiInterfaceWithAtmelPermission()
{
    return new RangeAdjInterface(m_pSCPIInterface, AdjustScpiValueFormatterFactory::createCom5003AdjFormatter());
}

void Com5003SenseInterface::setNotifierSenseMMode()
{
    notifierSenseMMode = SenseSystem::sMMode[m_nMMode];
}

void Com5003SenseInterface::setNotifierSenseChannelCat()
{
    int i;
    QString s;
    for (i = 0; i < m_ChannelList.count()-1; i++ )
        s += m_ChannelList.at(i)->getName() + ";";
    s += m_ChannelList.at(i)->getName();
    notifierSenseChannelCat = s;
}

void Com5003SenseInterface::unregisterSense()
{
    Com5003SenseChannel* pChannel;
    msgNrList.clear();
    for (int i = 0; i < 6; i++) {
        pChannel = m_ChannelList.at(i);
        unregister1Resource(m_rmConnection, NotZeroNumGen::getMsgNr(), QString("SENSE;%1;")
                                                                         .arg(pChannel->getName()));
    }
}

void Com5003SenseInterface::registerSense()
{
    QString s;
    // first we change the channels units and descriptions
    if (m_nMMode == SenseSystem::modeAC) {
        m_ChannelList.at(0)->setDescription(SenseSystem::sVoltageChannelDescription);
        m_ChannelList.at(0)->setUnit(s = "V");
        m_ChannelList.at(1)->setDescription(SenseSystem::sVoltageChannelDescription);
        m_ChannelList.at(1)->setUnit(s = "V");
        m_ChannelList.at(2)->setDescription(SenseSystem::sVoltageChannelDescription);
        m_ChannelList.at(2)->setUnit(s = "V");

        m_ChannelList.at(3)->setDescription(SenseSystem::sCurrentChannelDescription);
        m_ChannelList.at(3)->setUnit(s = "A");
        m_ChannelList.at(4)->setDescription(SenseSystem::sCurrentChannelDescription);
        m_ChannelList.at(4)->setUnit(s = "A");
        m_ChannelList.at(5)->setDescription(SenseSystem::sCurrentChannelDescription);
        m_ChannelList.at(5)->setUnit(s = "A");
    }
    else {
        for (qint32 i = 0; i < m_ChannelList.count(); i++) // for each channel
        {
            m_ChannelList.at(i)->setDescription(SenseSystem::sReferenceChannelDescription);
            m_ChannelList.at(i)->setUnit(s = "V");
        }
    }

    m_ctrlFactory->getMModeController()->setMeasMode(m_nMMode); // set the atmels mode too

    // here we do the rest of reconfiguring
    for (qint32 i = 0; i < m_ChannelList.count(); i++) {
        m_ChannelList.at(i)->setMMode(m_nMMode); // this indirectly changes the channnels alias
        QList<Com5003SenseRange*> list = m_ChannelList.at(i)->getRangeList();
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
