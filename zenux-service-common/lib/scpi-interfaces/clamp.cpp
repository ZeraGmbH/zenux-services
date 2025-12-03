#include "clamp.h"
#include "adjflags.h"
#include "clampsenserange.h"
#include "adjdataitemscpi.h"
#include "adjrangescpiclamp.h"
#include "adjrangescpi.h"
#include "zscpi_response_definitions.h"

enum Commands
{
    cmdSerial,
    cmdVersion,
    cmdType,
    cmdName,
    cmdFlashWrite,
    cmdFlashRead,
    cmdFlashReset,
    cmdChksum,
    cmdXMLWrite,
    cmdXMLRead,
    cmdStatAdjustment
};

cClamp::cClamp(PCBServer *server,
               SenseInterfaceCommon *senseInterface,
               const SenseSystem::cChannelSettings *chSettings,
               EepromI2cDeviceInterfacePtr adjMemory,
               quint8 ctrlChannelSecondary) :
    ScpiConnection(server->getSCPIInterface()),
    m_pSenseInterface(senseInterface),
    m_chSettings(chSettings),
    m_nCtrlChannelSecondary(ctrlChannelSecondary),
    m_adjReadWrite(std::move(adjMemory))
{
    quint8 type = readClampType();
    initClamp(type);
    // we need an adjustment interface in whatever state the clamp connected is
    addSystAdjInterface();

    // This blocks us from getting free of server
    // Problem: clamps are generated dynamically all other interfaces are setup once
    connect(this, &ScpiConnection::cmdExecutionDone, server, &PCBServer::sendProtoAnswer);
    connect(server, &PCBServer::removeSubscribers, this, &ScpiConnection::onRemoveSubscribers);

    if (type != undefined) {
        importClampAdjData();
        addSense();
        addSenseInterface();
    }
}

cClamp::~cClamp()
{
    removeAllRanges();
}

void cClamp::initSCPIConnection(QString)
{
}

QString cClamp::getChannelName() const
{
    return m_chSettings->m_nameMx;
}

QString cClamp::getChannelNameSecondary() const
{
    return m_sChannelNameSecondary;
}

QString cClamp::getSerial()
{
    return m_sSerial;
}

void cClamp::executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd)
{
    switch (cmdCode) {
    case cmdSerial:
        protoCmd->m_sOutput = scpiReadWriteSerial(protoCmd->m_sInput);
        break;
    case cmdVersion:
        protoCmd->m_sOutput = scpiReadWriteVersion(protoCmd->m_sInput);
        break;
    case cmdType:
        protoCmd->m_sOutput = scpiReadWriteType(protoCmd->m_sInput);
        break;
    case cmdName:
        protoCmd->m_sOutput = scpiReadName(protoCmd->m_sInput);
        break;
    case cmdFlashWrite:
        protoCmd->m_sOutput = scpiWriteFlash(protoCmd->m_sInput);
        break;
    case cmdFlashRead:
        protoCmd->m_sOutput = scpiReadFlash(protoCmd->m_sInput);
        break;
    case cmdFlashReset:
        protoCmd->m_sOutput = scpiResetFlash(protoCmd->m_sInput);
        break;
    case cmdChksum:
        protoCmd->m_sOutput = scpiReadChksum(protoCmd->m_sInput);
        break;
    case cmdXMLWrite:
        protoCmd->m_sOutput = scpiWriteXML(protoCmd->m_sInput);
        break;
    case cmdXMLRead:
        protoCmd->m_sOutput = scpiReadXML(protoCmd->m_sInput);
        break;
    case cmdStatAdjustment:
        protoCmd->m_sOutput = scpiReadAdjStatus(protoCmd->m_sInput);
        break;
    }
    if (protoCmd->m_bwithOutput) {
        emit cmdExecutionDone(protoCmd);
    }
}

bool cClamp::exportClampAdjData(QDateTime dateTimeWrite)
{
    QByteArray ba;
    QDataStream stream(&ba,QIODevice::ReadWrite);
    stream.setVersion(QDataStream::Qt_5_4);
    m_AdjDateTime = dateTimeWrite;
    quint32 count = 0;
    quint16 chksum = 0;

    stream << count; // first we write place holders for count and chksum this is the same for each adjflash object
    stream << chksum;
    stream << m_nType;
    stream << m_nFlags;
    stream << getClampTypeName(m_nType); // for sake of compatibilty
    stream << m_sVersion; // version
    stream << m_sSerial; //  serial
    stream << m_AdjDateTime.toString(Qt::TextDate); // date, time
    for(auto range : qAsConst(m_RangeList)) {
        stream << range->getRangeName();
        range->getJustData()->getAdjGroupData().toStream(stream);
    }
    for(auto range : qAsConst(m_RangeListSecondary)) {
        stream << range->getRangeName();
        range->getJustData()->getAdjGroupData().toStream(stream);
    }
    m_adjReadWrite.setData(ba);
    return m_adjReadWrite.writeData();
}

bool cClamp::importClampAdjData()
{
    if(m_adjReadWrite.readData()) {
        QByteArray ba = m_adjReadWrite.getData();
        QDataStream stream(&ba, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_5_4);

        stream.skipRawData(6);
        stream >> m_nType;
        stream >> m_nFlags;
        QString clampTypeNameDummy;
        stream >> clampTypeNameDummy; // for sake of compatibility
        stream >> m_sVersion;
        stream >> m_sSerial;
        QString dts;
        stream >> dts;

        int n = 0;
        m_AdjDateTime = QDateTime::fromString(dts);
        while (!stream.atEnd()) {
            QString rngName;
            stream >> rngName;
            AdjDataRange rangeAdjData;
            rangeAdjData.fromStream(stream);
            SenseRangeCommon* range = getRange(rngName);
            if (range != nullptr) {
                n++;
                range->getJustData()->setAdjGroupData(rangeAdjData);
            }
        }
        return (n == m_RangeList.count() + m_RangeListSecondary.count()); // it's ok if we found data for all ranges in our list
    }
    return false;
}

QString cClamp::exportXMLString(int indent)
{
    if(!isValidType())
        return QString();

    QDomDocument justqdom (QString("ClampAdjustmentData"));

    QDomElement pcbtag = justqdom.createElement( "CLAMP" );
    justqdom.appendChild( pcbtag );

    QDomElement tag = justqdom.createElement( "Type" );
    pcbtag.appendChild( tag );
    QDomText t = justqdom.createTextNode(getClampTypeName(m_nType));
    tag.appendChild( t );

    tag = justqdom.createElement( "VersionNumber" );
    pcbtag.appendChild( tag );
    t = justqdom.createTextNode(m_sVersion);
    tag.appendChild( t );

    tag = justqdom.createElement( "SerialNumber" );
    pcbtag.appendChild( tag );
    t = justqdom.createTextNode(m_sSerial);
    tag.appendChild( t );

    tag = justqdom.createElement( "Date" );
    pcbtag.appendChild( tag );
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QDate d = currentDateTime.date();
    t = justqdom.createTextNode(d.toString(Qt::TextDate));
    tag.appendChild( t );

    tag = justqdom.createElement( "Time" );
    pcbtag.appendChild( tag );
    QTime ti = currentDateTime.time();
    t = justqdom.createTextNode(ti.toString(Qt::TextDate));
    tag.appendChild( t );

    QDomElement adjtag = justqdom.createElement( "Adjustment" );
    pcbtag.appendChild( adjtag );

    QDomElement chksumtag = justqdom.createElement("Chksum");
    adjtag.appendChild(chksumtag);
    t = justqdom.createTextNode(QString("0x%1").arg(m_adjReadWrite.getChecksum(), 0, 16));
    chksumtag.appendChild(t);

    QDomElement typeTag = justqdom.createElement( "Sense");
    adjtag.appendChild(typeTag);

    for(auto range : qAsConst(m_RangeList)) {
        exportRangeXml(justqdom, typeTag, range);
    }
    for(auto range : qAsConst(m_RangeListSecondary)) {
        exportRangeXml(justqdom, typeTag, range);
    }
    return justqdom.toString(indent);
}

const SenseSystem::cChannelSettings *cClamp::getChannelSettings() const
{
    return m_chSettings;
}

bool cClamp::importXMLDocument(QDomDocument *qdomdoc, bool ignoreType)
{
    return importXMLDocumentStatic(qdomdoc, ignoreType,
                                   m_nType, m_sSerial, m_sVersion, m_RangeList, m_RangeListSecondary);
}

bool cClamp::importXMLDocumentStatic(QDomDocument *qdomdoc, bool ignoreType,
                                     quint8 clamptype, QString &serialNo, QString &version,
                                     const QList<SenseRangeCommon *> &rangeList, const QList<SenseRangeCommon *> &rangeListSecondary)
{
    QDateTime DateTime; // useless - TBD
    QDomDocumentType TheDocType = qdomdoc->doctype ();
    if(TheDocType.name() != QString("ClampAdjustmentData")) {
        qCritical("justdata import, wrong xml documentype");
        return false;
    }

    QDomElement rootElem = qdomdoc->documentElement();
    QDomNodeList nl = rootElem.childNodes();
    bool TypeOK = false;
    bool VersionNrOK = false;
    bool SerialNrOK = false;
    bool DateOK = false;
    bool TimeOK = false;

    for (int i = 0; i < nl.length() ; i++) {
        QDomNode qdNode = nl.item(i);
        QDomElement qdElem = qdNode.toElement();
        if ( qdElem.isNull() ) {
            qCritical("Justdata import, format error in xml file");
            return false;
        }
        QString tName = qdElem.tagName();
        if (tName == "Type") {
            if (ignoreType) {
                TypeOK = true;
            }
            else {
                TypeOK = qdElem.text() == getClampTypeName(clamptype);
                if (!TypeOK) {
                    qCritical("Justdata import, wrong type information in xml file");
                    return false;
                }
            }
        }
        else if (tName == "SerialNumber") {
            SerialNrOK = true;
            serialNo = qdElem.text();
        }
        else if (tName == "VersionNumber") {
            VersionNrOK = true;
            version = qdElem.text();
        }
        else if (tName=="Date") {
            QDate d = QDate::fromString(qdElem.text(),Qt::TextDate);
            DateTime.setDate(d);
            DateOK = true;
        }
        else if (tName=="Time") {
            QTime t = QTime::fromString(qdElem.text(),Qt::TextDate);
            DateTime.setTime(t);
            TimeOK = true;
        }
        else if (tName == "Adjustment") {
            if ( TypeOK && VersionNrOK && SerialNrOK && DateOK && TimeOK) {
                bool done = false;
                QDomNodeList adjChildNl = qdElem.childNodes();
                for (qint32 j = 0; j < adjChildNl.length(); j++) {
                    qdNode = adjChildNl.item(j);
                    if (qdNode.toElement().tagName() == "Sense") {// we look for the sense entry

                        done = true;
                        QDomNodeList sensNl = qdNode.childNodes(); // we iterate over all ranges
                        SenseRangeCommon* rngPtr = nullptr;
                        for (qint32 j = 0; j < sensNl.length(); j++) {
                            QString Name;

                            QDomNode RangeJustNode = sensNl.item(j);
                            qdElem = RangeJustNode.toElement();
                            QString tName = qdElem.tagName();
                            if (tName == "Range") {
                                QDomNodeList rngJustNl = RangeJustNode.childNodes();
                                for (qint32 k = 0; k < rngJustNl.length(); k++) {
                                    QDomNode RangeJustNode = rngJustNl.item(k);

                                    qdElem = RangeJustNode.toElement();
                                    tName = qdElem.tagName();
                                    if (tName == "Name") {
                                        Name = qdElem.text();
                                        rngPtr = getRangeStatic(Name, rangeList, rangeListSecondary);
                                    }

                                    AdjDataItemScpi* pJustData = nullptr;
                                    if (rngPtr != 0)
                                        pJustData = rngPtr->getJustData()->getAdjInterface(tName);
                                    if (pJustData) {
                                        QDomNodeList jdataNl = RangeJustNode.childNodes();
                                        for (qint32 k = 0; k < jdataNl.count(); k++) {
                                            QDomNode jTypeNode = jdataNl.item(k);
                                            QString jTypeName = jTypeNode.toElement().tagName();
                                            QString jdata = jTypeNode.toElement().text();
                                            if (jTypeName == "Status") {
                                                pJustData->statusFromString(jdata);
                                            }
                                            else if (jTypeName == "Coefficients") {
                                                pJustData->coefficientsFromString(jdata);
                                            }
                                            if (jTypeName == "Nodes") {
                                                pJustData->nodesFromString(jdata);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                if (!done) {
                    return done;
                }
            }
            else {
                qCritical("Justdata import, xml file contains strange data");
                return false;
            }
        }
        else {
            qCritical("justdata import, xml file contains strange data");
            return false;
        }
    }
    return true;
}

bool cClamp::importXmlForSerialNo(QDomDocument *qdomdoc, QString &serialNo)
{
    serialNo = ClampDefaultSerNo;
    QString version = ClampDefaultVersion;
    quint8 type = undefined;
    return importXMLDocumentStatic(qdomdoc, true,
                                   type, serialNo, version,
                                   QList<SenseRangeCommon *>(), QList<SenseRangeCommon *>());
}

bool cClamp::importXMLDocument(QDomDocument *qdomdoc)
{
    return importXMLDocument(qdomdoc, false);
}

bool cClamp::isValidType()
{
    return getClampTypeName(undefined) != getClampTypeName(m_nType);
}

quint8 cClamp::getAdjStatus()
{
    // Out of order clamps - e.g with non-working EEPROM
    // have an empty range list. To avoid reporting them
    // hey I can't do anything for you but I am adjusted,
    // bail out early
    if(m_RangeList.count() == 0) {
        return Adjustment::notAdjusted;
    }

    quint8 stat = 255;
    for(auto range : qAsConst(m_RangeList))
        stat &= range->getAdjustmentStatus80Mask();
    for(auto range : qAsConst(m_RangeListSecondary))
        stat &= range->getAdjustmentStatus80Mask();
    if ((stat & Adjustment::AdjustedMask)== 0)
        return Adjustment::notAdjusted;
    else
        return Adjustment::adjusted;
}

cClamp::ClampTypes cClamp::readClampType()
{
    if(m_adjReadWrite.readData()) {
        QByteArray ba = m_adjReadWrite.getData();
        quint8 type;
        QDataStream stream(&ba, QIODevice::ReadWrite);
        stream.setVersion(QDataStream::Qt_5_4);
        stream.skipRawData(6);
        stream >> type;
        return ClampTypes(type);
    }
    return undefined;
}

void cClamp::createLEM1000VRanges(const PermissionStructAdj &permissions)
{
    const quint16 dcClampMask = m_pSenseInterface->rangeFlagsExternDc();
    m_sChannelNameSecondary = m_pSenseInterface->getChannelByCtrlChannelNo(m_nCtrlChannelSecondary);
    AdjRangeScpiClamp* clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(m_sChannelNameSecondary, QString("8V")), 118.6, permissions);
    m_RangeListSecondary.append(new ClampSenseRange(m_scpiInterface, "C1000V", true, 1000.0, 3535110.0, 3535110.0 * 1.25, 0x01 /*8V*/, dcClampMask, clampJustData));
}

void cClamp::createVDE1400VRanges(const PermissionStructAdj &permissions)
{
    const quint16 dcClampMask = m_pSenseInterface->rangeFlagsExternDc();
    m_sChannelNameSecondary = m_pSenseInterface->getChannelByCtrlChannelNo(m_nCtrlChannelSecondary);
    AdjRangeScpiClamp* clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(m_sChannelNameSecondary, QString("8V")), 148.1, permissions);
    m_RangeListSecondary.append(new ClampSenseRange(m_scpiInterface, "C1400V", true, 1400.0, 3966009.0, 3966009.0 * 1.25, 0x01 /*8V*/, dcClampMask, clampJustData));
}

void cClamp::initClamp(quint8 type)
{
    m_nType = type;
    AdjRangeScpiClamp* clampJustData;
    PermissionStructAdj permissionsOffsetAllowedAlways;
    permissionsOffsetAllowedAlways.funcAllowAdjOffset = PermissionFunctions::allowAlways;
    const quint16 acClampFlags = m_pSenseInterface->rangeFlagsExtern();
    const quint16 dcClampMask = m_pSenseInterface->rangeFlagsExternDc();
    const QString channelMName = m_chSettings->m_nameMx;
    switch (type)
    {
    case CL120A:
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("2V")), 48.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,  "C100A", true, 100.0, 2953735.0, 3692169.0, 11, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("1V")), 48.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,   "C50A", true,  50.0, 2953735.0, 3692169.0, 12, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("200mV")), 48.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,   "C10A", true,  10.0, 2796203.0, 3495254.0, 14, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("100mV")), 48.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,    "C5A", true,   5.0, 3495253.0, 4369066.0, 15, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("20mV")), 48.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,    "C1A", true,   1.0, 2796203.0, 3495254.0, 17, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("10mV")), 48.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface, "C500mA",  true,  0.5, 3495253.0, 4369066.0, 18, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("2mV")), 48.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface, "C100mA",  true,  0.1, 2796203.0, 3495254.0, 20, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("2mV")), 48.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,  "C50mA",  true, 0.05, 1398101.0, 1747626.0, 20, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("2mV")), 48.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,  "C10mA",  true, 0.01,  279620.0,  349525.0, 20, acClampFlags, clampJustData));
        break;

    case CL300A:
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("5V")), 120.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,  "C300A", true, 300.0, 2097152.0, 2097152.0, 10, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("2V")), 120.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,  "C150A", true, 150.0, 1772241.0, 2215302.0, 11, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("500mV")), 120.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,   "C30A", true,  30.0, 1772241.0, 2215302.0, 13, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("200mV")), 120.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,   "C15A", true,  15.0, 1677722.0, 2097153.0, 14, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("50mV")), 120.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,    "C3A", true,   3.0, 1677722.0, 2097153.0, 16, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("20mV")), 120.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,  "C1.5A", true,   1.5, 1677722.0, 2097153.0, 17, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("5mV")), 120.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface, "C300mA", true,   0.3, 1677722.0, 2097153.0, 19, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("2mV")), 120.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface, "C150mA", true,  0.15, 1677722.0, 2097153.0, 20, acClampFlags, clampJustData));
        break;

    case CL1000A:
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("1V")), 1200.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface, "C1000A", true, 1000.0, 2362988.0, 2362988.0, 12, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("500mV")), 1200.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,  "C300A", true,  300.0, 1772241.0, 2215302.0, 13, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("100mV")), 1200.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,  "C100A", true,  100.0, 2796203.0, 3495254.0, 15, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("50mV")), 1200.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,   "C30A", true,   30.0, 1677722.0, 2097153.0, 16, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("10mV")), 1200.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,   "C10A", true,   10.0, 2796203.0, 3495254.0, 18, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("5mV")), 1200.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,    "C3A", true,    3.0, 1677722.0, 2097153.0, 19, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("2mV")), 1200.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,    "C1A", true,    1.0, 1118481.0, 1398109.0, 20, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("2mV")), 1200.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface, "C300mA", true,    0.3,  335544.0,  419430.0, 20, acClampFlags, clampJustData));
        break;

    case EMOB32:
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("2V")), 1000.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,   "C50A", true, 50.0, 1772241.0, 2215301.0, 11, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("500mV")), 1000.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,   "C10A", true, 10.0, 1772241.0, 2215301.0, 13, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("200mV")), 1000.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,    "C5A", true,  5.0, 1677722.0, 2097152.0, 14, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("50mV")), 1000.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,    "C1A", true,  1.0, 1677722.0, 2097152.0, 16, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("20mV")), 1000.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface, "C500mA", true,  0.5, 1677722.0, 2097152.0, 17, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("5mV")), 1000.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface, "C100mA", true,  0.1, 1677722.0, 2097152.0, 19, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("2mV")), 1000.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface, "C50mA",  true, 0.05, 1677722.0, 2097152.0, 20, acClampFlags, clampJustData));
        break;

    case EMOB200DC:
        // I
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("5V")), 1000.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface, "C200A", true, 200.0, 2516582.0, 2516582.0 * 1.25, 0x0A, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("500mV")), 1000.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,  "C20A", true,  20.0, 2126689.0, 2126689.0 * 1.25, 0x0D, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("50mV")), 1000.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,   "C2A", true,   2.0, 2013266.0, 2013266.0 * 1.25, 0x10, dcClampMask, clampJustData));

        // This clamp has a secondary channnel U
        m_sChannelNameSecondary = m_pSenseInterface->getChannelByCtrlChannelNo(m_nCtrlChannelSecondary);
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(m_sChannelNameSecondary, QString("8V")), 121.0);
        m_RangeListSecondary.append(new ClampSenseRange(m_scpiInterface, "C1000V", true, 1000.0, 3466367.0, 3466367.0 * 1.25, 1 /*8V*/, dcClampMask, clampJustData));
        break;

    case EMOB80:
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("5V")), 1000.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,   "C100A", true, 100.0, 2097152.0, 2621440.0, 10, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("2V")), 1000.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,   "C50A", true,   50.0, 1772241.0, 2215301.0, 11, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("500mV")), 1000.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,   "C10A", true,   10.0, 1772241.0, 2215301.0, 13, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("200mV")), 1000.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,    "C5A", true,    5.0, 1677722.0, 2097152.0, 14, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("50mV")), 1000.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,    "C1A", true,    1.0, 1677722.0, 2097152.0, 16, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("20mV")), 1000.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface, "C500mA", true,    0.5, 1677722.0, 2097152.0, 17, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("5mV")), 1000.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface, "C100mA", true,    0.1, 1677722.0, 2097152.0, 19, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("2mV")), 1000.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,  "C50mA", true,   0.05, 1677722.0, 2097152.0, 20, acClampFlags, clampJustData));
        break;

    case RC3000:
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("5V")), 1000.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface, "C3000A", true, 3000.0, 2516582.0, 2516582.0, 10, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("500mV")), 1000.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,  "C300A", true,  300.0, 2126689.0, 2126689.0 * 1.25, 13, acClampFlags, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("50mV")), 1000.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,   "C30A", true,   30.0, 2013266.0, 2013266.0 * 1.25, 16, acClampFlags, clampJustData));
        break;

    case CL200ADC1000VDC: // LEM U+I
        // I
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("8V")), 500.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,  "C200A", true, 200.0, 2516582.0, 2516582.0 * 1.25, 0x09, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("5V")), 500.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,  "C100A", true, 100.0, 2516582.0, 2516582.0 * 1.25, 0x0A, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("2V")), 500.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,   "C50A", true, 50.0, 2126689.0, 2126689.0 * 1.25, 0x0B, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("1V")), 500.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,   "C20A", true, 20.0, 1701351.0, 1701351.0 * 1.25, 0x0C, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("500mV")), 500.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,   "C10A", true, 10.0, 2126689.0, 2126689.0 * 1.25, 0x0D, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("200mV")), 500.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,    "C5A", true,  5.0, 2013266.0, 2013266.0 * 1.25, 0x0E, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("100mV")), 500.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,    "C2A", true,  2.0, 2013266.0, 2013266.0 * 1.25, 0x0F, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("50mV")), 500.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,    "C1A", true,  1.0, 2013266.0, 2013266.0 * 1.25, 0x10, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("20mV")), 500.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface, "C500mA", true,  0.5, 2013266.0, 2013266.0 * 1.25, 0x11, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("10mV")), 500.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface, "C200mA", true,  0.2, 2013266.0, 2013266.0 * 1.25, 0x12, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("5mV")), 500.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface, "C100mA", true,  0.1, 2013266.0, 2013266.0 * 1.25, 0x13, dcClampMask, clampJustData));
        // U
        createLEM1000VRanges(permissionsOffsetAllowedAlways);
        break;

    case CL1000VDC: // LEM U only
        createLEM1000VRanges(permissionsOffsetAllowedAlways);
        break;

    case CL8ADC1000VDC: // TESLA U+I 8A/1000V
        // I
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("1V")), 30.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,    "C8A", true, 8.0, 3024625.0, 3024625.0 * 1.25, 0x0C, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("500mV")), 30.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,    "C4A", true, 4.0, 3780781.0, 3780781.0 * 1.25, 0x0D, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("200mV")), 30.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,    "C2A", true, 2.0, 3579139.0, 3579139.0 * 1.25, 0x0E, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("100mV")), 30.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface, "C800mA", true, 0.8, 3579139.0, 3579139.0 * 1.25, 0x0F, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("50mV")), 30.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface, "C400mA", true, 0.4, 3579139.0, 3579139.0 * 1.25, 0x10, dcClampMask, clampJustData));
        // U
        createLEM1000VRanges(permissionsOffsetAllowedAlways);
        break;

    case CL800ADC1000VDC: // TESLA U+I 800A/1000V
        // I
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("2V")), 1500.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface, "C800A", true, 800.0, 3024625.0, 3024625.0 * 1.30, 0x0B, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("1V")), 1500.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface, "C400A", true, 400.0, 3024625.0, 3024625.0 * 1.25, 0x0C, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("500mV")), 1500.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface, "C200A", true, 200.0, 3780781.0, 3780781.0 * 1.25, 0x0D, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("200mV")), 1500.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,  "C80A", true,  80.0, 2863312.0, 2863312.0 * 1.25, 0x0E, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("100mV")), 1500.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,  "C40A", true,  40.0, 3579139.0, 3579139.0 * 1.25, 0x0F, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("50mV")), 1500.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,  "C20A", true,  20.0, 3579139.0, 3579139.0 * 1.25, 0x10, dcClampMask, clampJustData));
        // U
        createLEM1000VRanges(permissionsOffsetAllowedAlways);
        break;

    case EMOB500DC:
        // I
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("5V")), 2000.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface, "C500A", true, 500.0, 3145728.0, 3145728.0 * 1.25, 0x0A, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("500mV")), 2000.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,  "C50A", true,  50.0, 2658362.0, 2658362.0 * 1.25, 0x0D, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("50mV")), 2000.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,   "C5A", true,   5.0, 2516582.0, 2516582.0 * 1.25, 0x10, dcClampMask, clampJustData));

        // This clamp has a secondary channnel U
        m_sChannelNameSecondary = m_pSenseInterface->getChannelByCtrlChannelNo(m_nCtrlChannelSecondary);
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(m_sChannelNameSecondary, QString("8V")), 121.0);
        m_RangeListSecondary.append(new ClampSenseRange(m_scpiInterface, "C1000V", true, 1000.0, 3466367.0, 3466367.0 * 1.25, 1 /*8V*/, dcClampMask, clampJustData));
        break;

    case EMOB750DC:
        // I
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("2V")), 1500.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface, "C750A", true, 750.0, 5316723.0, 5316723.0 *1.066, 0x0B, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("200mV")), 1500.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,  "C75A", true,  75.0, 5033165.0, 5033165.0 * 1.25, 0x0E, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("20mV")), 1500.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface, "C7.5A", true,  7.50, 5033165.0, 5033165.0 * 1.25, 0x11, dcClampMask, clampJustData));

        // This clamp has a secondary channnel U
        m_sChannelNameSecondary = m_pSenseInterface->getChannelByCtrlChannelNo(m_nCtrlChannelSecondary);
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(m_sChannelNameSecondary, QString("8V")), 121.0);
        m_RangeListSecondary.append(new ClampSenseRange(m_scpiInterface, "C1000V", true, 1000.0, 3466367.0, 3466367.0 * 1.25, 1 /*8V*/, dcClampMask, clampJustData));
        break;

    case CL8ADC1400VDC: // VDE U+I 8A/1400V
        // I
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("1V")), 30.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,    "C8A", true, 8.0, 3024625.0, 3024625.0 * 1.25, 0x0C, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("500mV")), 30.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,    "C4A", true, 4.0, 3780781.0, 3780781.0 * 1.25, 0x0D, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("200mV")), 30.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,    "C2A", true, 2.0, 3579139.0, 3579139.0 * 1.25, 0x0E, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("100mV")), 30.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface, "C800mA", true, 0.8, 3579139.0, 3579139.0 * 1.25, 0x0F, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("50mV")), 30.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface, "C400mA", true, 0.4, 3579139.0, 3579139.0 * 1.25, 0x10, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("20mV")), 30.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface, "C200mA", true, 0.2, 3579139.0, 3579139.0 * 1.25, 0x11, dcClampMask, clampJustData));
        // U
        createVDE1400VRanges(permissionsOffsetAllowedAlways);
        break;

    case CL800ADC1400VDC: // VDE U+I 800A/1400V
        // I
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("2V")), 1500.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface, "C800A", true, 800.0, 3024625.0, 3024625.0 * 1.30, 0x0B, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("1V")), 1500.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface, "C400A", true, 400.0, 3024625.0, 3024625.0 * 1.25, 0x0C, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("500mV")), 1500.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface, "C200A", true, 200.0, 3780781.0, 3780781.0 * 1.25, 0x0D, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("200mV")), 1500.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,  "C80A", true,  80.0, 2863312.0, 2863312.0 * 1.25, 0x0E, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("100mV")), 1500.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,  "C40A", true,  40.0, 3579139.0, 3579139.0 * 1.25, 0x0F, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("50mV")), 1500.0);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,  "C20A", true,  20.0, 3579139.0, 3579139.0 * 1.25, 0x10, dcClampMask, clampJustData));
        // U
        createVDE1400VRanges(permissionsOffsetAllowedAlways);
        break;

    case CL1200ADC1400VDC: // LEM-3 1200A/1400V
        // I
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("5V")), 1500.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface, "C1200A", true, 1200.0, 2684355.0, 2684355.0 * 1.25, 0x0A, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("2V")), 1500.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface, "C800A", true, 800.0, 3024625.0, 3024625.0 * 1.30, 0x0B, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("1V")), 1500.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface, "C400A", true, 400.0, 3024625.0, 3024625.0 * 1.25, 0x0C, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("500mV")), 1500.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface, "C200A", true, 200.0, 3780781.0, 3780781.0 * 1.25, 0x0D, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("200mV")), 1500.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,  "C80A", true,  80.0, 2863312.0, 2863312.0 * 1.25, 0x0E, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("100mV")), 1500.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,  "C40A", true,  40.0, 3579139.0, 3579139.0 * 1.25, 0x0F, dcClampMask, clampJustData));
        clampJustData = new AdjRangeScpiClamp(m_scpiInterface, m_pSenseInterface->getRange(channelMName, QString("50mV")), 1500.0, permissionsOffsetAllowedAlways);
        m_RangeList.append(new ClampSenseRange(m_scpiInterface,  "C20A", true,  20.0, 3579139.0, 3579139.0 * 1.25, 0x10, dcClampMask, clampJustData));
        // U
        createVDE1400VRanges(permissionsOffsetAllowedAlways);
        break;

    case CL1400VDC:
        createVDE1400VRanges(permissionsOffsetAllowedAlways);
        break;
    }

    if(!m_RangeList.isEmpty() || !m_RangeListSecondary.isEmpty())
        qInfo("Clamp type: %s", qPrintable(getClampTypeName(type)));
    else
        qCritical("Unkown clamp type %u - no ranges set!", type);
}

bool cClamp::isValidClampType(quint8 type)
{
    return type > undefined && type < anzCL;
}

QString cClamp::getClampTypeName(quint8 type)
{
    QString CLName;
    switch (type)
    {
    case CL120A:
        CLName = QString("CL120A");
        break;
    case CL300A:
        CLName = QString("CL300A");
        break;
    case CL1000A:
        CLName  = QString("CL1000A");
        break;
    case EMOB32:
        CLName = QString("EMOB32");
        break;
    case EMOB200DC:
        CLName = QString("EMOB200DC");
        break;
    case EMOB80:
        CLName = QString("EMOB80");
        break;
    case RC3000:
        CLName = QString("RC3000");
        break;
    case CL200ADC1000VDC:
        CLName = QString("CL200ADC1000VDC");
        break;
    case CL1000VDC:
        CLName = QString("CL1000VDC");
        break;
    case CL8ADC1000VDC:
        CLName = QString("CL8ADC1000VDC");
        break;
    case CL800ADC1000VDC:
        CLName = QString("CL800ADC1000VDC");
        break;
    case EMOB500DC:
        CLName = QString("EMOB500DC");
        break;
    case EMOB750DC:
        CLName = QString("EMOB750DC");
        break;
    case CL8ADC1400VDC:
        CLName = QString("CL8ADC1400VDC");
        break;
    case CL800ADC1400VDC:
        CLName = QString("CL800ADC1400VDC");
        break;
    case CL1200ADC1400VDC:
        CLName = QString("CL1200ADC1400VDC");
        break;
    case CL1400VDC:
        CLName = QString("CL1400VDC");
        break;
    default:
        CLName = QString("Undefined");
    }
    return CLName;
}

void cClamp::addSense()
{
    m_pSenseInterface->getChannel(getChannelName())->addRangeList(m_RangeList);
    if(!m_sChannelNameSecondary.isEmpty()) {
        m_pSenseInterface->getChannel(m_sChannelNameSecondary)->addRangeList(m_RangeListSecondary);
    }
}

void cClamp::addSenseInterface()
{
    for(SenseRangeCommon *range : qAsConst(m_RangeList)) {
        range->initSCPIConnection(QString("SENSE:%1").arg(getChannelName()));
        connect(range, &SenseRangeCommon::cmdExecutionDone, this, &cClamp::cmdExecutionDone);
    }
    for(SenseRangeCommon *range : qAsConst(m_RangeListSecondary)) {
        range->initSCPIConnection(QString("SENSE:%1").arg(m_sChannelNameSecondary));
        connect(range, &SenseRangeCommon::cmdExecutionDone, this, &cClamp::cmdExecutionDone);
    }
}

void cClamp::addSystAdjInterface()
{
    addSystAdjInterfaceChannel(getChannelName());
    if(!m_sChannelNameSecondary.isEmpty()) {
        addSystAdjInterfaceChannel(m_sChannelNameSecondary);
    }
}

void cClamp::addSystAdjInterfaceChannel(QString channelName)
{
    QString cmdParent = QString("SYSTEM:CLAMP:%1").arg(channelName);
    addDelegate(cmdParent, "SERIALNUMBER", SCPI::isQuery | SCPI::isCmdwP, m_scpiInterface, cmdSerial);
    addDelegate(cmdParent, "VERSION", SCPI::isQuery | SCPI::isCmdwP, m_scpiInterface, cmdVersion);
    addDelegate(cmdParent, "TYPE",SCPI::isQuery | SCPI::isCmdwP, m_scpiInterface, cmdType);
    addDelegate(cmdParent, "NAME",SCPI::isQuery, m_scpiInterface, cmdName);

    cmdParent = QString("SYSTEM:ADJUSTMENT:CLAMP:%1:FLASH").arg(channelName);
    addDelegate(cmdParent,"WRITE", SCPI::isCmd, m_scpiInterface, cmdFlashWrite);
    addDelegate(cmdParent,"READ", SCPI::isCmd, m_scpiInterface, cmdFlashRead);
    addDelegate(cmdParent,"CHKSUM", SCPI::isQuery, m_scpiInterface, cmdChksum);
    addDelegate(cmdParent,"RESET", SCPI::isCmd, m_scpiInterface, cmdFlashReset);

    cmdParent = QString("SYSTEM:ADJUSTMENT:CLAMP:%1:XML").arg(channelName);
    addDelegate(cmdParent,"WRITE", SCPI::isCmd, m_scpiInterface, cmdXMLWrite);
    addDelegate(cmdParent,"READ", SCPI::isCmd, m_scpiInterface, cmdXMLRead);

    // Do we need this really? There ws hoplug indication introduced long time ago
    cmdParent = QString("STATUS:CLAMP:%1").arg(channelName);
    addDelegate(cmdParent, "ADJUSTMENT", SCPI::isQuery, m_scpiInterface, cmdStatAdjustment);
}

SenseRangeCommon *cClamp::getRange(QString name)
{
    return getRangeStatic(name, m_RangeList, m_RangeListSecondary);
}

SenseRangeCommon *cClamp::getRangeStatic(QString name, const QList<SenseRangeCommon*> &rangeList, const QList<SenseRangeCommon*> &rangeListSecondary)
{
    SenseRangeCommon* rangeFound = nullptr;
    for(auto range : rangeList) {
        if (range->getRangeName() == name) {
            rangeFound = range;
            break;
        }
    }
    if(!rangeFound) {
        for(auto range : rangeListSecondary) {
            if (range->getRangeName() == name) {
                rangeFound = range;
                break;
            }
        }
    }
    return rangeFound;
}

void cClamp::removeAllRanges()
{
    if(m_pSenseInterface) {
        // first we remove the already set ranges
        m_pSenseInterface->getChannel(getChannelName())->removeRangeList(m_RangeList);
        if(!m_sChannelNameSecondary.isEmpty()) {
            m_pSenseInterface->getChannel(m_sChannelNameSecondary)->removeRangeList(m_RangeListSecondary);
        }
    }
    // then we delete them what automatically destroys their interfaces
    for(auto* range : qAsConst(m_RangeList)) {
        delete range; // the ClampSenseRange objects will also remove their interfaces including that for adjustment data
    }
    m_RangeList.clear();
    for(auto* range : qAsConst(m_RangeListSecondary)) {
        delete range;
    }
    m_RangeListSecondary.clear();
}

void cClamp::exportRangeXml(QDomDocument &justqdom, QDomElement &typeTag, SenseRangeCommon *range)
{
    QDomElement rtag = justqdom.createElement("Range");
    typeTag.appendChild( rtag );

    QDomElement nametag = justqdom.createElement("Name");
    rtag.appendChild(nametag);

    QDomText t = justqdom.createTextNode(range->getRangeName());
    nametag.appendChild( t );

    QDomElement gpotag;
    const QStringList listAdjTypes = QStringList() << "Gain" << "Phase" << "Offset";
    for(const auto &adjType : listAdjTypes) {
        gpotag = justqdom.createElement(adjType);
        rtag.appendChild(gpotag);
        AdjDataItemScpi* adjDataInterface = range->getJustData()->getAdjInterface(adjType);
        QDomElement tag = justqdom.createElement("Status");
        QString jdata = adjDataInterface->statusToString();
        t = justqdom.createTextNode(jdata);
        gpotag.appendChild(tag);
        tag.appendChild(t);
        tag = justqdom.createElement("Coefficients");
        gpotag.appendChild(tag);
        jdata = adjDataInterface->coefficientsToString();
        t = justqdom.createTextNode(jdata);
        tag.appendChild(t);
        tag = justqdom.createElement("Nodes");
        gpotag.appendChild(tag);
        jdata = adjDataInterface->nodesToString();
        t = justqdom.createTextNode(jdata);
        tag.appendChild(t);
    }
}

QString cClamp::scpiReadWriteSerial(QString& scpi)
{
    QString answer;
    cSCPICommand cmd =scpi;
    if (cmd.isQuery()) {
        answer = m_sSerial;
    }
    else {
        if (cmd.isCommand(1)) {
            QString serial = cmd.getParam(0);
            if (serial.length() <= 10) {
                m_sSerial = serial;
                answer = ZSCPI::scpiAnswer[ZSCPI::ack];
            }
            else {
                answer = ZSCPI::scpiAnswer[ZSCPI::errval];
            }
        }
        else {
            answer = ZSCPI::scpiAnswer[ZSCPI::nak];
        }
    }
    return answer;
}


QString cClamp::scpiReadWriteVersion(QString &scpi)
{
    QString answer;
    cSCPICommand cmd =scpi;
    if (cmd.isQuery()) {
        answer = m_sVersion;
    }
    else {
        if (cmd.isCommand(1)) {
            QString version = cmd.getParam(0);
            if (version.length() == 4) {
                m_sVersion = version;
                answer = ZSCPI::scpiAnswer[ZSCPI::ack];
            }
            else {
                answer = ZSCPI::scpiAnswer[ZSCPI::errval];
            }
        }
        else {
            answer = ZSCPI::scpiAnswer[ZSCPI::nak];
        }
    }
    return answer;
}


QString cClamp::scpiReadWriteType(QString& scpi)
{
    QString answer;
    cSCPICommand cmd =scpi;
    if (cmd.isQuery()) {
        answer = QString("%1").arg(m_nType);
    }
    else {
        if (cmd.isCommand(1)) {
            quint8 type;
            type = cmd.getParam(0).toInt();
            if ( (type > undefined) && (type < anzCL)) {
                removeAllRanges();
                initClamp(type);
                if (exportClampAdjData(QDateTime::currentDateTime())) {
                    addSense();
                    addSenseInterface();
                    answer = ZSCPI::scpiAnswer[ZSCPI::ack];
                }
                else {
                    answer = ZSCPI::scpiAnswer[ZSCPI::errexec];
                }
            }
            else {
                answer = ZSCPI::scpiAnswer[ZSCPI::errval];
            }
        }
        else {
            answer = ZSCPI::scpiAnswer[ZSCPI::nak];
        }
    }
    return answer;
}

QString cClamp::scpiReadName(QString& scpi)
{
    QString answer;
    cSCPICommand cmd =scpi;
    if (cmd.isQuery()) {
        answer = getClampTypeName(m_nType);
    }
    else {
        answer = ZSCPI::scpiAnswer[ZSCPI::nak];
    }
    return answer;
}

QString cClamp::scpiWriteFlash(QString& scpi)
{
    QString answer;
    cSCPICommand cmd = scpi;
    if (cmd.isCommand(1) && (cmd.getParam(0) == "")) {
        if (exportClampAdjData(QDateTime::currentDateTime())) {
            answer = ZSCPI::scpiAnswer[ZSCPI::ack];
        }
        else {
            answer = ZSCPI::scpiAnswer[ZSCPI::errexec];
        }
    }
    else {
        answer = ZSCPI::scpiAnswer[ZSCPI::nak];
    }
    return answer;
}

QString cClamp::scpiReadFlash(QString& scpi)
{
    QString answer;
    cSCPICommand cmd = scpi;
    if (cmd.isCommand(1) && (cmd.getParam(0) == "")) {
        if (readClampType() == m_nType) { // we first look whether the type matches
            importClampAdjData();
            answer = ZSCPI::scpiAnswer[ZSCPI::ack];
        }
        else {
            answer = ZSCPI::scpiAnswer[ZSCPI::errexec];
        }
    }
    else {
        answer = ZSCPI::scpiAnswer[ZSCPI::nak];
    }
    return answer;
}

QString cClamp::scpiResetFlash(QString &scpi)
{
    QString answer;
    cSCPICommand cmd = scpi;
    if (cmd.isCommand(1) && (cmd.getParam(0) == "")) {
        if (m_adjReadWrite.resetData()) {
            answer = ZSCPI::scpiAnswer[ZSCPI::ack];
        }
        else {
            answer = ZSCPI::scpiAnswer[ZSCPI::errexec];
        }
    }
    else {
        answer = ZSCPI::scpiAnswer[ZSCPI::nak];
    }
    return answer;
}

QString cClamp::scpiReadChksum(QString& scpi)
{
    QString answer;
    cSCPICommand cmd = scpi;
    if (cmd.isQuery()) {
        answer = QString("0x%1").arg(m_adjReadWrite.getChecksum(),0,16); // hex output
    }
    else {
        answer = ZSCPI::scpiAnswer[ZSCPI::nak];
    }
    return answer;
}

QString cClamp::scpiWriteXML(QString &scpi)
{
    QString answer;
    cSCPICommand cmd = scpi;
    if (cmd.isCommand(1)) {
        QString filename = cmd.getParam(0);
        if (exportAdTojXMLFile(filename)) {
            answer = ZSCPI::scpiAnswer[ZSCPI::ack];
        }
        else {
            answer = ZSCPI::scpiAnswer[ZSCPI::errexec];
        }
    }
    else {
        answer = ZSCPI::scpiAnswer[ZSCPI::nak];
    }
    return answer;
}

QString cClamp::scpiReadXML(QString &scpi)
{
    QString answer;
    cSCPICommand cmd = scpi;
    if (cmd.isCommand(1)) {
        QString filename = cmd.getParam(0);
        if (importAdjXMLFile(filename)) {
            answer = ZSCPI::scpiAnswer[ZSCPI::ack];
        }
        else {
            answer = ZSCPI::scpiAnswer[ZSCPI::errexec];
        }
    }
    else {
        answer = ZSCPI::scpiAnswer[ZSCPI::nak];
    }
    return answer;
}

QString cClamp::scpiReadAdjStatus(QString &scpi)
{
    QString answer;
    cSCPICommand cmd = scpi;
    if (cmd.isQuery()) {
        answer = QString("%1").arg(getAdjStatus()); // hex output
    }
    else {
        answer = ZSCPI::scpiAnswer[ZSCPI::nak];
    }
    return answer;
}
