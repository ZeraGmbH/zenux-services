#include "com5003senseinterface.h"
#include "adjustmentflags.h"
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
    SenseInterfaceCommon(scpiInterface,
                           i2cSettings,
                           systemInfo,
                           ctrlFactory),
    m_rmConnection(rmConnection),
    m_ethSettings(ethSettings)
{

    // Init with bad defaults so coder's bugs pop up
    m_nSerialStatus = Adjustment::wrongSNR;

    m_currSenseMode = "AC";
    m_availSenseModesHash["AC"] = SenseSystem::modeAC;
    m_availSenseModesHash["REF"] = SenseSystem::modeREF;

    m_ctrlFactory->getMModeController()->setMeasMode(SenseSystem::modeAC); // set the atmels mode too
    setNotifierSenseMMode();

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

    setNotifierSenseChannelCat(); // only prepared for !!! since we don't have hot plug for measuring channels yet

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

void Com5003SenseInterface::initSCPIConnection(QString leadingNodes)
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

SenseChannelCommon *Com5003SenseInterface::getChannel(QString &name)
{
    SenseChannelCommon *channelFound = nullptr;
    for(auto channel : qAsConst(m_channelList)) {
        if(channel->getName() == name) {
            channelFound = channel;
            break;
        }
    }
    return channelFound;
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
    case SenseSystem::cmdStatAdjustment:
        protoCmd->m_sOutput = scpiReadAdjStatus(protoCmd->m_sInput);
        if (protoCmd->m_bwithOutput)
            emit cmdExecutionDone(protoCmd);
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

    for(auto channel : qAsConst(m_channelList)) {
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


void Com5003SenseInterface::exportAdjData(QDataStream &stream, QDateTime dateTimeWrite)
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
            // This was stolen from MT and that just stores direct ranges (no clamp ranges)
            // Once COM supports clamps, we have to revisit
            if (true) {
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


Com5003SenseInterface::SetModeModeResult Com5003SenseInterface::setSenseMode(QString mode)
{
    if(mode == m_currSenseMode)
        return done;
    if(!m_availSenseModesHash.contains(mode))
        return failed;
    m_currSenseMode = mode;
    return start_statemachine_required;
}

void Com5003SenseInterface::scpiReadWriteMMode(cProtonetCommand *protoCmd)
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

QString Com5003SenseInterface::m_ReadSenseGroupCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;
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

QString Com5003SenseInterface::m_InitSenseAdjData(QString &sInput)
{
    cSCPICommand cmd = sInput;
    // cmd.isCommand(0) is not correct but we leave it for compatibility
    if ( cmd.isCommand(0) || (cmd.isCommand(1) && (cmd.getParam(0) == ""))) {
        bool enable;
        if (m_ctrlFactory->getPermissionCheckController()->hasPermission(enable)) {
            if (enable) {
                for (int i = 0; i < m_channelList.count(); i++)
                    m_channelList.at(i)->initJustData();

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
                for (int i = 0; i < m_channelList.count(); i++)
                    m_channelList.at(i)->computeJustData();
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

QString Com5003SenseInterface::scpiReadAdjStatus(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return  QString("%1").arg(getAdjustmentStatus());
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

RangeAdjInterface *Com5003SenseInterface::createJustScpiInterfaceWithAtmelPermission()
{
    return new RangeAdjInterface(m_pSCPIInterface, AdjustScpiValueFormatterFactory::createCom5003AdjFormatter());
}

void Com5003SenseInterface::unregisterSense()
{
    SenseChannelCommon* pChannel;
    msgNrList.clear();
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
