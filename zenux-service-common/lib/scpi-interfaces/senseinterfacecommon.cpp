#include "senseinterfacecommon.h"
#include "adjustmentflags.h"
#include "notzeronumgen.h"
#include "zscpi_response_definitions.h"
#include <i2cmultiplexerfactory.h>

QString SenseInterfaceCommon::m_version = "V1.00";

SenseInterfaceCommon::SenseInterfaceCommon(cSCPI *scpiInterface,
                                           I2cSettings *i2cSettings,
                                           SystemInfo *systemInfo,
                                           AbstractFactoryI2cCtrlPtr ctrlFactory) :
    cResource(scpiInterface),
    m_adjReadWrite(i2cSettings->getDeviceNode(),
                     i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress),
                     I2cMultiplexerFactory::createNullMuxer()),
    m_systemInfo(systemInfo),
    m_ctrlFactory(ctrlFactory)
{
}

SenseInterfaceCommon::~SenseInterfaceCommon()
{
    for(auto channel : qAsConst(m_channelList))
        delete channel;
    m_channelList.clear();
}

quint8 SenseInterfaceCommon::getAdjustmentStatus()
{
    quint8 adjustmentStatusMask = Adjustment::adjusted;
    // Loop adjustment state for all channels
    for(auto channel : qAsConst(m_channelList)) {
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

SenseChannelCommon *SenseInterfaceCommon::getChannel(QString &name)
{
    for(auto channel : qAsConst(m_channelList))
        if(channel->getName() == name)
            return channel;
    return nullptr;
}

QString SenseInterfaceCommon::getChannelByCtrlChannelNo(quint16 ctrlChannel)
{
    for(auto channel : qAsConst(m_channelList))
        if(channel->getCtrlChannel() == ctrlChannel)
            return channel->getName();
    return QString();
}

SenseRangeCommon *SenseInterfaceCommon::getRange(QString channelName, QString rangeName)
{
    SenseChannelCommon *channelFound = getChannel(channelName);
    if(channelFound)
        return channelFound->getRange(rangeName);
    return nullptr;
}

void SenseInterfaceCommon::computeSenseAdjData()
{
    for(auto channel : qAsConst(m_channelList))
        channel->computeJustData();
}

void SenseInterfaceCommon::registerResource(RMConnection *rmConnection, quint16 port)
{
    msgNrList.clear();
    for(auto channel : qAsConst(m_channelList)) {
        register1Resource(rmConnection, NotZeroNumGen::getMsgNr(), QString("SENSE;%1;1;%2;%3;")
                                                                       .arg(channel->getName(), channel->getDescription())
                                                                       .arg(port));
    }
    // additional we register measuring mode switch as resource
    QString measModeDescription = QString("Measuring mode switch %1").arg(getSenseModesSortedById().join(","));
    register1Resource(rmConnection, NotZeroNumGen::getMsgNr(), QString("SENSE;MMODE;1;%1;%2;")
                                                                   .arg(measModeDescription)
                                                                   .arg(port));
}

void SenseInterfaceCommon::initSCPIConnection(QString leadingNodes)
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

bool SenseInterfaceCommon::importAdjData()
{
    if(m_adjReadWrite.importAdjFlash()) {
        QByteArray ba = m_adjReadWrite.getAdjData();
        QDataStream streamForAdjReader(&ba, QIODevice::ReadOnly);
        streamForAdjReader.setVersion(QDataStream::Qt_5_4);
        m_adjustmentReader.extractDeviceInfos(streamForAdjReader);

        QDataStream stream(&ba, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_5_4);

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
    return false;
}

quint16 SenseInterfaceCommon::getAdjChecksum()
{
    return m_adjReadWrite.getChecksum();
}

bool SenseInterfaceCommon::exportAdjData(QDateTime dateTimeWrite)
{
    QByteArray ba;
    QDataStream stream(&ba,QIODevice::ReadWrite);
    stream.setVersion(QDataStream::Qt_5_4);

    quint32 count = 0;
    quint16 chksum = 0;

    stream << count; // first we write place holders for count and chksum this is the same for each adjflash object
    stream << chksum;

    stream << "ServerVersion";
    stream << getAdjExportedVersion();
    stream << m_systemInfo->getDeviceName().toStdString().c_str(); // leiterkarten name aus atmel gelesen
    stream << m_systemInfo->getDeviceVersion().toStdString().c_str(); // geräte name versionsnummern ...
    stream << m_systemInfo->getSerialNumber().toStdString().c_str(); // seriennummer
    stream << dateTimeWrite.toString(Qt::TextDate).toStdString().c_str(); // datum,uhrzeit
    for(auto channel : qAsConst(m_channelList)) {
        for(auto range : channel->getRangeList()) {
            if (isRangePartOfAdjXmlExport(range)) {
                QString spec = QString("%1:%2:%3")
                                   .arg("SENSE")
                                   .arg(channel->getName())
                                   .arg(range->getRangeName());

                stream << spec.toLatin1();
                range->getJustData()->Serialize(stream);
            }
        }
    }
    m_adjReadWrite.setAdjData(ba);
    return m_adjReadWrite.exportAdjFlash();
}

bool SenseInterfaceCommon::importXMLDocument(QDomDocument* qdomdoc)
{
    QDomDocumentType TheDocType = qdomdoc->doctype ();
    if(TheDocType.name() != getXmlType()) {
        qCritical("Justdata import: wrong xml documentype");
        return false;
    }
    QDomElement rootElem = qdomdoc->documentElement();
    QDomNodeList nl = rootElem.childNodes();
    bool TypeOK = false;
    bool SerialNrOK = false;
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
            TypeOK = qdElem.text() == getPcbName();
            if (!TypeOK) {
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
        // TODO date & time -> EEPROM
        else if (tName=="Date") {
            /*QDate d = QDate::fromString(qdElem.text(),Qt::TextDate);
            bool DateOK = d.isValid();*/
        }
        else if (tName=="Time") {
            /*QTime t = QTime::fromString(qdElem.text(),Qt::TextDate);
            bool TimeOK = TimeOK = t.isValid();*/
        }
        else if (tName == "Adjustment") {
            if (SerialNrOK && TypeOK) {
                QDomNodeList adjChildNl = qdElem.childNodes();
                for (qint32 j = 0; j < adjChildNl.length(); j++) {
                    qdNode = adjChildNl.item(j);
                    QString tagName = qdNode.toElement().tagName();
                    if (tagName == "Chksum")
                        ChksumOK = true; // we don't read it actually because if something was changed outside ....
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

QString SenseInterfaceCommon::exportXMLString(int indent)
{
    QDomDocument justqdom(getXmlType());

    QDomElement pcbtag = justqdom.createElement("PCB");
    justqdom.appendChild( pcbtag );

    QDomElement tag = justqdom.createElement( "Type" );
    pcbtag.appendChild( tag );
    QDomText t = justqdom.createTextNode(getPcbName());
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
    t = justqdom.createTextNode(QString("0x%1").arg(m_adjReadWrite.getChecksum(), 0, 16));
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
            if (isRangePartOfAdjXmlExport(range)) {
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

void SenseInterfaceCommon::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case SenseSystem::cmdVersion:
        protoCmd->m_sOutput = scpiReadVersion(protoCmd->m_sInput);
        break;
    case SenseSystem::cmdMMode:
        handleScpiReadWriteMMode(protoCmd);
        return;
    case SenseSystem::cmdMModeCat:
        protoCmd->m_sOutput = scpiReadMModeCatalog(protoCmd->m_sInput);
        break;
    case SenseSystem::cmdChannelCat:
        protoCmd->m_sOutput = scpiReadSenseChannelCatalog(protoCmd->m_sInput);
        break;
    case SenseSystem::cmdGroupCat:
        protoCmd->m_sOutput = scpiReadSenseGroupCatalog(protoCmd->m_sInput);
        break;
    case SenseSystem::initAdjData:
        protoCmd->m_sOutput = scpiInitSenseAdjDataAllChannelRanges(protoCmd->m_sInput);
        break;
    case SenseSystem::computeAdjData:
        protoCmd->m_sOutput = scpiComputeSenseAdjDataAllChannelRanges(protoCmd->m_sInput);
        break;
    case SenseSystem::cmdStatAdjustment:
        protoCmd->m_sOutput = scpiReadAdjStatus(protoCmd->m_sInput);
        break;
    }
    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

QString SenseInterfaceCommon::scpiReadVersion(QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return m_version;
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseInterfaceCommon::scpiReadMModeCatalog(QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return getSenseModesSortedById().join(";");
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseInterfaceCommon::scpiReadSenseChannelCatalog(QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return m_notifierSenseChannelCat.getString();
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseInterfaceCommon::scpiInitSenseAdjDataAllChannelRanges(QString &scpi)
{
    cSCPICommand cmd = scpi;
    // cmd.isCommand(0) is not correct but we leave it for compatibility
    if ( cmd.isCommand(0) || (cmd.isCommand(1) && (cmd.getParam(0) == ""))) {
        bool enable;
        if (m_ctrlFactory->getPermissionCheckController()->hasPermission(enable)) {
            if (enable) {
                for(auto channel : qAsConst(m_channelList))
                    channel->initJustData();
                return ZSCPI::scpiAnswer[ZSCPI::ack];
            }
            return ZSCPI::scpiAnswer[ZSCPI::erraut];
        }
        return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseInterfaceCommon::scpiComputeSenseAdjDataAllChannelRanges(QString &scpi)
{
    cSCPICommand cmd = scpi;
    if ( cmd.isCommand(1) && (cmd.getParam(0) == "") ) {
        bool enable;
        if (m_ctrlFactory->getPermissionCheckController()->hasPermission(enable)) {
            if (enable) {
                computeSenseAdjData();
                return ZSCPI::scpiAnswer[ZSCPI::ack];
            }
            return ZSCPI::scpiAnswer[ZSCPI::erraut];
        }
        return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseInterfaceCommon::scpiReadAdjStatus(QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return  QString("%1").arg(getAdjustmentStatus());
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

void SenseInterfaceCommon::setNotifierSenseMMode()
{
    m_notifierSenseMMode = m_currSenseMode;
}

void SenseInterfaceCommon::setNotifierSenseChannelCat()
{
    // only prepared for !!! since we don't have hot plug for measuring channels yet
    int i;
    QString s;
    for (i = 0; i < m_channelList.count()-1; i++ )
        s += m_channelList.at(i)->getName() + ";";
    s += m_channelList.at(i)->getName();
    m_notifierSenseChannelCat = s;
}

QStringList SenseInterfaceCommon::getSenseModesSortedById()
{
    const QStringList modeNames = m_availSenseModesHash.keys();
    QMap<int, QString> sortedModes; // original COM implementation was sorted (MT hash random)
    for(const auto &modeName : modeNames)
        sortedModes[m_availSenseModesHash[modeName]] = modeName;
    return sortedModes.values();
}
