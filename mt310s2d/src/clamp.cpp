#include "clamp.h"
#include "adjustment.h"
#include "justdata.h"
#include "mt310s2d.h"
#include "mt310s2dglobal.h"
#include "senserange.h"
#include "clampjustdata.h"
#include <protonetcommand.h>
#include "settings/i2csettings.h"
#include <syslog.h>
#include <i2cutils.h>
#include <i2cmuxerscopedonoff.h>

cClamp::cClamp() :
    cSCPIConnection(nullptr) // TODO get rid of dummy clamp
{
}

cClamp::cClamp(cMT310S2dServer *server, QString channelName, quint8 ctrlChannel, I2cMuxerInterface::Ptr i2cMuxer, quint8 ctrlChannelSecondary) :
    cAdjFlash(server->m_pI2CSettings->getDeviceNode(),
              server->m_pI2CSettings->getI2CAdress(i2cSettings::clampflash),
              i2cMuxer),
    cSCPIConnection(server->getSCPIInterface()),
    m_pSenseInterface(server->m_pSenseInterface),
    m_sChannelName(channelName),
    m_i2cMuxAdress(server->m_pI2CSettings->getI2CAdress(i2cSettings::flashmux)),
    m_nCtrlChannel(ctrlChannel),
    m_nCtrlChannelSecondary(ctrlChannelSecondary)
{
    m_sSerial = "1234567890"; // our default serial number
    m_sVersion = "unknown";
    m_nFlags = 0;
    m_nType = undefined;

    quint8 type = readClampType();
    initClamp(type);
    // we need an adjustment interface in whatever state the clamp connected is
    addSystAdjInterface();
    connect(this, SIGNAL(cmdExecutionDone(cProtonetCommand*)), server, SLOT(sendAnswer(cProtonetCommand*)));
    if (type != undefined) {
        importAdjFlash();
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

QString cClamp::getChannelName()
{
    return m_sChannelName;
}

QString cClamp::getChannelNameSecondary()
{
    return m_sChannelNameSecondary;
}

QString cClamp::getSerial()
{
    return m_sSerial;
}

void cClamp::executeCommand(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode) {
    case clamp::cmdSerial:
        protoCmd->m_sOutput = handleScpiReadWriteSerial(protoCmd->m_sInput);
        break;
    case clamp::cmdVersion:
        protoCmd->m_sOutput = handleScpiReadWriteVersion(protoCmd->m_sInput);
        break;
    case clamp::cmdType:
        protoCmd->m_sOutput = handleScpiReadWriteType(protoCmd->m_sInput);
        break;
    case clamp::cmdName:
        protoCmd->m_sOutput = handleScpiReadName(protoCmd->m_sInput);
        break;
    case clamp::cmdFlashWrite:
        protoCmd->m_sOutput = handleScpiWriteFlash(protoCmd->m_sInput);
        break;
    case clamp::cmdFlashRead:
        protoCmd->m_sOutput = handleScpiReadFlash(protoCmd->m_sInput);
        break;
    case clamp::cmdFlashReset:
        protoCmd->m_sOutput = handleScpiResetFlash(protoCmd->m_sInput);
        break;
    case clamp::cmdChksum:
        protoCmd->m_sOutput = handleScpiReadChksum(protoCmd->m_sInput);
        break;
    case clamp::cmdXMLWrite:
        protoCmd->m_sOutput = handleScpiWriteXML(protoCmd->m_sInput);
        break;
    case clamp::cmdXMLRead:
        protoCmd->m_sOutput = handleScpiReadXML(protoCmd->m_sInput);
        break;
    case clamp::cmdStatAdjustment:
        protoCmd->m_sOutput = handleScpiReadAdjStatus(protoCmd->m_sInput);
        break;
    }
    if (protoCmd->m_bwithOutput) {
        emit cmdExecutionDone(protoCmd);
    }
}

void cClamp::exportAdjData(QDataStream &stream)
{
    m_AdjDateTime = QDateTime::currentDateTime();
    stream << m_nType;
    stream << m_nFlags;
    stream << getClampTypeName(m_nType); // for sake of compatibilty
    stream << m_sVersion; // version
    stream << m_sSerial; //  serial
    stream << m_AdjDateTime.toString(Qt::TextDate); // date, time
    QString spec;
    for(auto range : m_RangeList) {
        spec = range->getName();
        stream << spec;
        range->getJustData()->Serialize(stream);
    }
    for(auto range : m_RangeListSecondary) {
        spec = range->getName();
        stream << spec;
        range->getJustData()->Serialize(stream);
    }
}

bool cClamp::importAdjData(QDataStream &stream)
{
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
        cSenseRange* range = getRange(rngName);
        if (range != 0) {
            n++;
            range->getJustData()->Deserialize(stream);
        }
        else {
            cMT310S2JustData *dummy = new cMT310S2JustData(m_pSCPIInterface); // if we did not find this range....something has changed
            dummy->Deserialize(stream); // we read the data from stream to keep it in flow
            delete dummy;
        }
    }
    return (n == m_RangeList.count() + m_RangeListSecondary.count()); // it's ok if we found data for all ranges in our list
}

QString cClamp::exportXMLString(int indent)
{
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
    t = justqdom.createTextNode(QString("0x%1").arg(m_nChecksum,0,16));
    chksumtag.appendChild(t);

    QDomElement typeTag = justqdom.createElement( "Sense");
    adjtag.appendChild(typeTag);

    for(auto range : m_RangeList) {
        exportRangeXml(justqdom, typeTag, range);
    }
    for(auto range : m_RangeListSecondary) {
        exportRangeXml(justqdom, typeTag, range);
    }
    return justqdom.toString(indent);
}

bool cClamp::importXMLDocument(QDomDocument *qdomdoc, bool ignoreType)
{
    QDateTime DateTime;
    QDomDocumentType TheDocType = qdomdoc->doctype ();
    if  (TheDocType.name() != QString("ClampAdjustmentData")) {
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

    for (int i = 0; i < nl.length() ; i++) {
        QDomNode qdNode = nl.item(i);
        QDomElement qdElem = qdNode.toElement();
        if ( qdElem.isNull() ) {
            syslog(LOG_ERR,"justdata import, format error in xml file\n");
            return false;
        }
        QString tName = qdElem.tagName();
        if (tName == "Type") {
            if (ignoreType) {
                TypeOK = true;
            }
            else {
                TypeOK = qdElem.text() == getClampTypeName(m_nType);
                if (!TypeOK) {
                    syslog(LOG_ERR,"justdata import, wrong type information in xml file\n");
                    return false;
                }
            }
        }
        else if (tName == "SerialNumber") {
            SerialNrOK = true;
            m_sSerial = qdElem.text();
        }
        else if (tName == "VersionNumber") {
           VersionNrOK = true;
           m_sVersion = qdElem.text();
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
                    qDebug() << qdNode.toElement().tagName();
                    if (qdNode.toElement().tagName() == "Sense") {// we look for the sense entry

                        done = true;
                        QDomNodeList sensNl = qdNode.childNodes(); // we iterate over all ranges
                        cSenseRange* rngPtr = nullptr;
                        for (qint32 j = 0; j < sensNl.length(); j++) {
                            QString Name;

                            QDomNode RangeJustNode = sensNl.item(j);
                            qdElem = RangeJustNode.toElement();
                            QString tName = qdElem.tagName();
                            qDebug() << tName;

                            if (tName == "Range") {
                                QDomNodeList rngJustNl = RangeJustNode.childNodes();
                                for (qint32 k = 0; k < rngJustNl.length(); k++) {
                                    QDomNode RangeJustNode = rngJustNl.item(k);

                                    qdElem = RangeJustNode.toElement();
                                    tName = qdElem.tagName();
                                    qDebug() << tName;

                                    if (tName == "Name") {
                                        Name = qdElem.text();
                                        qDebug() << Name;
                                        rngPtr = getRange(Name);
                                    }

                                    cJustData* pJustData = nullptr;
                                    if (rngPtr != 0) {
                                        if (tName == "Gain") {
                                            pJustData = rngPtr->getJustData()->m_pGainCorrection;
                                        }
                                        else if (tName == "Phase") {
                                            pJustData = rngPtr->getJustData()->m_pPhaseCorrection;
                                        }
                                        else if (tName == "Offset") {
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
                                            else if (jTypeName == "Coefficients") {
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
                if (!done) {
                    return done;
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
    return true;
}

bool cClamp::importXMLDocument(QDomDocument *qdomdoc)
{
    return importXMLDocument(qdomdoc, false);
}

quint8 cClamp::getAdjustmentStatus()
{
    // Out of order clamps - e.g with non-working EEPROM
    // have an empty range list. To avoid reporting them
    // hey I can't do anything for you but I am adjusted,
    // bail out early
    if(m_RangeList.count() == 0) {
        return Adjustment::notAdjusted;
    }

    quint8 stat = 255;
    for(auto range : m_RangeList) {
        stat &= range->getAdjustmentStatus();
    }
    for(auto range : m_RangeListSecondary) {
        stat &= range->getAdjustmentStatus();
    }
    if ((stat & JustData::Justified)== 0) {
        return Adjustment::notAdjusted;
    }
    else {
        return Adjustment::adjusted;
    }
}

ClampTypes cClamp::readClampType()
{
    I2cMuxerScopedOnOff i2cMuxOnOff(m_i2cMuxer);
    QByteArray ba;
    if (readFlash(ba)) { // flash data could be read with correct chksum
        quint8 type;
        QDataStream stream(&ba, QIODevice::ReadWrite);
        stream.setVersion(QDataStream::Qt_5_4);
        stream.skipRawData(6);
        stream >> type;
        return ClampTypes(type);
    }
    return undefined;
}

void cClamp::initClamp(quint8 type)
{
    m_nType = type;
    cClampJustData* clampJustData;
    switch (type)
    {
    case CL120A:
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("2V")), 48.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface,  "C100A",  "C100A", true, 100.0, 2953735.0, 3692169.0, 8388607.0, 11, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("1V")), 48.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface,   "C50A",   "C50A", true,  50.0, 2953735.0, 3692169.0, 8388607.0, 12, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("200mV")), 48.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface,   "C10A",   "C10A", true,  10.0, 2796203.0, 3495254.0, 8388607.0, 14, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("100mV")), 48.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface,    "C5A",    "C5A", true,   5.0, 3495253.0, 4369066.0, 8388607.0, 15, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("20mV")), 48.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface,    "C1A",    "C1A", true,   1.0, 2796203.0, 3495254.0, 8388607.0, 17, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("10mV")), 48.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface, "C500mA", "C500mA", true,   0.5, 3495253.0, 4369066.0, 8388607.0, 18, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("2mV")), 48.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface, "C100mA", "C100mA", true,   0.1, 2796203.0, 3495254.0, 8388607.0, 20, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("2mV")), 48.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface,  "C50mA",  "C50mA", true,  0.05, 1398101.0, 1747626.0, 8388607.0, 20, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("2mV")), 48.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface,  "C10mA",  "C10mA", true,  0.01,  279620.0,  349525.0, 8388607.0, 20, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));

        break;

    case CL300A:
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("5V")), 120.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface,  "C300A",  "C300A", true, 300.0, 2097152.0, 2097152.0, 8388607.0, 10, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("2V")), 120.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface,  "C150A",  "C150A", true, 150.0, 1772241.0, 2215302.0, 8388607.0, 11, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("500mV")), 120.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface,   "C30A",   "C30A", true,  30.0, 1772241.0, 2215302.0, 8388607.0, 13, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("200mV")), 120.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface,   "C15A",   "C15A", true,  15.0, 1677722.0, 2097153.0, 8388607.0, 14, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("50mV")), 120.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface,    "C3A",    "C3A", true,   3.0, 1677722.0, 2097153.0, 8388607.0, 16, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("20mV")), 120.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface,  "C1.5A",  "C1.5A", true,   1.5, 1677722.0, 2097153.0, 8388607.0, 17, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("5mV")), 120.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface, "C300mA", "C300mA", true,   0.3, 1677722.0, 2097153.0, 8388607.0, 19, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("2mV")), 120.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface, "C150mA", "C150mA", true,  0.15, 1677722.0, 2097153.0, 8388607.0, 20, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));

        break;

    case CL1000A:
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("1V")), 1200.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface, "C1000A", "C1000A", true, 1000.0, 2362988.0, 2362988.0, 8388607.0, 12, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("500mV")), 1200.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface,  "C300A",  "C300A", true,  300.0, 1772241.0, 2215302.0, 8388607.0, 13, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("100mV")), 1200.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface,  "C100A",  "C100A", true,  100.0, 2796203.0, 3495254.0, 8388607.0, 15, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("50mV")), 1200.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface,   "C30A",   "C30A", true,   30.0, 1677722.0, 2097153.0, 8388607.0, 16, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("10mV")), 1200.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface,   "C10A",   "C10A", true,   10.0, 2796203.0, 3495254.0, 8388607.0, 18, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("5mV")), 1200.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface,    "C3A",    "C3A", true,    3.0, 1677722.0, 2097153.0, 8388607.0, 19, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("2mV")), 1200.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface,    "C1A",    "C1A", true,    1.0, 1118481.0, 1398109.0, 8388607.0, 20, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("2mV")), 1200.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface, "C300mA", "C300mA", true,    0.3,  335544.0,  419430.0, 8388607.0, 20, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));

        break;

    case EMOB32:
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("2V")), 1000.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface, "C50A", "C50A", true, 50.0, 1772241.0, 2215301.0, 8388607.0, 11, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("500mV")), 1000.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface, "C10A", "C10A", true, 10.0, 1772241.0, 2215301.0, 8388607.0, 13, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("200mV")), 1000.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface,  "C5A",  "C5A", true,  5.0, 1677722.0, 2097152.0, 8388607.0, 14, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("50mV")), 1000.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface,  "C1A",  "C1A", true,  1.0, 1677722.0, 2097152.0, 8388607.0, 16, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("20mV")), 1000.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface, "C500mA", "C500mA", true, 0.5, 1677722.0, 2097152.0, 8388607.0, 17, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("5mV")), 1000.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface, "C100mA", "C100mA", true, 0.1, 1677722.0, 2097152.0, 8388607.0, 19, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("2mV")), 1000.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface, "C50mA", "C50mA", true, 0.05, 1677722.0, 2097152.0, 8388607.0, 20, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));

        break;
    case EMOB200DC:
    {
        // VERY IMPORTANT:
        // We need to understand what the implementation state of modeDC is.
        // Without modeAC cold plug makes new EMOB200DC unavailable while hotplug works as expected!!!
        quint16 commonMask = SenseSystem::modeDC | SenseSystem::modeAC;
        // I
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("5V")), 1000.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface, "C200A", "C200A", true, 200.0, 2516582.0, 2516582.0 * 1.25, 8388607.0, 0x0A, commonMask | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("500mV")), 1000.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface, "C20A", "C20A", true, 20.0, 2126689.0, 2126689.0 * 1.25, 8388607.0, 0x0D, commonMask | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("50mV")), 1000.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface, "C2A", "C2A", true, 2.0, 2013266.0, 2013266.0 * 1.25, 8388607.0, 0x10, commonMask | SenseSystem::Clamp, clampJustData));

        // This clamp has a secondary channnel U
        m_sChannelNameSecondary = m_pSenseInterface->getChannelSystemName(m_nCtrlChannelSecondary);
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelNameSecondary, QString("8V")), 121.0);
        m_RangeListSecondary.append(new cSenseRange(m_pSCPIInterface, "C1000V", "C1000V", true, 1000.0, 3466367.0, 3466367.0 * 1.25, 8388607.0, 1 /*8V*/, commonMask | SenseSystem::Clamp, clampJustData));

        break;
    }
    case EMOB80:
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("5V")), 1000.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface, "C100A", "C100A", true, 100.0, 2097152.0, 2621440.0, 8388607.0, 10, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("2V")), 1000.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface, "C50A", "C50A", true, 50.0, 1772241.0, 2215301.0, 8388607.0, 11, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("500mV")), 1000.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface, "C10A", "C10A", true, 10.0, 1772241.0, 2215301.0, 8388607.0, 13, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("200mV")), 1000.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface,  "C5A",  "C5A", true,  5.0, 1677722.0, 2097152.0, 8388607.0, 14, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("50mV")), 1000.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface,  "C1A",  "C1A", true,  1.0, 1677722.0, 2097152.0, 8388607.0, 16, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("20mV")), 1000.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface, "C500mA", "C500mA", true, 0.5, 1677722.0, 2097152.0, 8388607.0, 17, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("5mV")), 1000.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface, "C100mA", "C100mA", true, 0.1, 1677722.0, 2097152.0, 8388607.0, 19, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));
        clampJustData = new cClampJustData(m_pSCPIInterface, m_pSenseInterface->getRange(m_sChannelName, QString("2mV")), 1000.0);
        m_RangeList.append(new cSenseRange(m_pSCPIInterface, "C50mA", "C50mA", true, 0.05, 1677722.0, 2097152.0, 8388607.0, 20, SenseSystem::modeAC | SenseSystem::Clamp, clampJustData));

        break;
    }
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

       default:
            CLName = QString("Undefined");
    }
    return CLName;
}

void cClamp::addSense()
{
    m_pSenseInterface->getChannel(m_sChannelName)->addRangeList(m_RangeList);
    if(!m_sChannelNameSecondary.isEmpty()) {
        m_pSenseInterface->getChannel(m_sChannelNameSecondary)->addRangeList(m_RangeListSecondary);
    }
}

void cClamp::addSenseInterface()
{
    for(cSenseRange *range : m_RangeList) {
        range->initSCPIConnection(QString("SENSE:%1").arg(m_sChannelName));
        connect(range, &cSenseRange::cmdExecutionDone, this, &cClamp::cmdExecutionDone);
    }
    for(cSenseRange *range : m_RangeListSecondary) {
        range->initSCPIConnection(QString("SENSE:%1").arg(m_sChannelNameSecondary));
        connect(range, &cSenseRange::cmdExecutionDone, this, &cClamp::cmdExecutionDone);
    }
}

void cClamp::addSystAdjInterface()
{
    addSystAdjInterfaceChannel(m_sChannelName);
    if(!m_sChannelNameSecondary.isEmpty()) {
        addSystAdjInterfaceChannel(m_sChannelNameSecondary);
    }
}

void cClamp::addSystAdjInterfaceChannel(QString channelName)
{
    QString cmdParent;
    cSCPIDelegate* delegate;

    cmdParent = QString("SYSTEM:CLAMP:%1").arg(channelName);
    delegate = new cSCPIDelegate(cmdParent, "SERIALNUMBER", SCPI::isQuery | SCPI::isCmdwP, m_pSCPIInterface, clamp::cmdSerial);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(cmdParent, "VERSION", SCPI::isQuery | SCPI::isCmdwP, m_pSCPIInterface, clamp::cmdVersion);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(cmdParent, "TYPE",SCPI::isQuery | SCPI::isCmdwP, m_pSCPIInterface, clamp::cmdType);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(cmdParent, "NAME",SCPI::isQuery, m_pSCPIInterface, clamp::cmdName );
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));

    cmdParent = QString("SYSTEM:ADJUSTMENT:CLAMP:%1:FLASH").arg(channelName);
    delegate = new cSCPIDelegate(cmdParent,"WRITE", SCPI::isCmd, m_pSCPIInterface, clamp::cmdFlashWrite);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(cmdParent,"READ", SCPI::isCmd, m_pSCPIInterface, clamp::cmdFlashRead);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(cmdParent,"CHKSUM", SCPI::isQuery, m_pSCPIInterface, clamp::cmdChksum);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(cmdParent,"RESET", SCPI::isCmd, m_pSCPIInterface, clamp::cmdFlashReset);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));

    cmdParent = QString("SYSTEM:ADJUSTMENT:CLAMP:%1:XML").arg(channelName);
    delegate = new cSCPIDelegate(cmdParent,"WRITE", SCPI::isCmd, m_pSCPIInterface, clamp::cmdXMLWrite);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(cmdParent,"READ", SCPI::isCmd, m_pSCPIInterface, clamp::cmdXMLRead);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));

    cmdParent = QString("STATUS:CLAMP:%1").arg(channelName);
    delegate = new cSCPIDelegate(cmdParent, "ADJUSTMENT", SCPI::isQuery, m_pSCPIInterface, clamp::cmdStatAdjustment);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
}

cSenseRange* cClamp::getRange(QString name)
{
    cSenseRange* rangeFound = nullptr;
    for(auto range : m_RangeList) {
        if (range->getName() == name) {
            rangeFound = range;
            break;
        }
    }
    if(!rangeFound) {
        for(auto range : m_RangeListSecondary) {
            if (range->getName() == name) {
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
        m_pSenseInterface->getChannel(m_sChannelName)->removeRangeList(m_RangeList);
        if(!m_sChannelNameSecondary.isEmpty()) {
            m_pSenseInterface->getChannel(m_sChannelNameSecondary)->removeRangeList(m_RangeListSecondary);
        }
    }
    // then we delete them what automatically destroys their interfaces
    for(auto* range : m_RangeList) {
        delete range; // the cSenseRange objects will also remove their interfaces including that for adjustment data
    }
    m_RangeList.clear();
    for(auto* range : m_RangeListSecondary) {
        delete range;
    }
    m_RangeListSecondary.clear();
}

void cClamp::exportRangeXml(QDomDocument &justqdom, QDomElement &typeTag, cSenseRange *range)
{
    QDomElement rtag = justqdom.createElement( "Range" );
    typeTag.appendChild( rtag );

    QDomElement nametag = justqdom.createElement( "Name" );
    rtag.appendChild(nametag);

    QDomText t = justqdom.createTextNode(range->getName());
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

QString cClamp::handleScpiReadWriteSerial(QString& scpiCmdStr)
{
    QString answer;
    cSCPICommand cmd =scpiCmdStr;
    if (cmd.isQuery()) {
        answer = m_sSerial;
    }
    else {
        if (cmd.isCommand(1)) {
            QString serial = cmd.getParam(0);
            if (serial.length() <= 10) {
                m_sSerial = serial;
                answer = SCPI::scpiAnswer[SCPI::ack];
            }
            else {
                answer = SCPI::scpiAnswer[SCPI::errval];
            }
        }
        else {
            answer = SCPI::scpiAnswer[SCPI::nak];
        }
    }
    return answer;
}


QString cClamp::handleScpiReadWriteVersion(QString &scpiCmdStr)
{
    QString answer;
    cSCPICommand cmd =scpiCmdStr;
    if (cmd.isQuery()) {
        answer = m_sVersion;
    }
    else {
        if (cmd.isCommand(1)) {
            QString version = cmd.getParam(0);
            if (version.length() == 4) {
                m_sVersion = version;
                answer = SCPI::scpiAnswer[SCPI::ack];
            }
            else {
                answer = SCPI::scpiAnswer[SCPI::errval];
            }
        }
        else {
            answer = SCPI::scpiAnswer[SCPI::nak];
        }
    }
    return answer;
}


QString cClamp::handleScpiReadWriteType(QString& scpiCmdStr)
{
    QString answer;
    cSCPICommand cmd =scpiCmdStr;
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
                if (exportAdjFlash()) {
                    addSense();
                    addSenseInterface();
                    answer = SCPI::scpiAnswer[SCPI::ack];
                }
                else {
                    answer = SCPI::scpiAnswer[SCPI::errexec];
                }
            }
            else {
                answer = SCPI::scpiAnswer[SCPI::errval];
            }
        }
        else {
            answer = SCPI::scpiAnswer[SCPI::nak];
        }
    }
    return answer;
}

QString cClamp::handleScpiReadName(QString& scpiCmdStr)
{
    QString answer;
    cSCPICommand cmd =scpiCmdStr;
    if (cmd.isQuery()) {
        answer = getClampTypeName(m_nType);
    }
    else {
        answer = SCPI::scpiAnswer[SCPI::nak];
    }
    return answer;
}

QString cClamp::handleScpiWriteFlash(QString& scpiCmdStr)
{
    QString answer;
    cSCPICommand cmd = scpiCmdStr;
    if (cmd.isCommand(1) && (cmd.getParam(0) == "")) {
        if (exportAdjFlash()) {
            answer = SCPI::scpiAnswer[SCPI::ack];
        }
        else {
            answer = SCPI::scpiAnswer[SCPI::errexec];
        }
    }
    else {
        answer = SCPI::scpiAnswer[SCPI::nak];
    }
    return answer;
}

QString cClamp::handleScpiReadFlash(QString& scpiCmdStr)
{
    QString answer;
    cSCPICommand cmd = scpiCmdStr;
    if (cmd.isCommand(1) && (cmd.getParam(0) == "")) {
        if (readClampType() == m_nType) { // we first look whether the type matches
            importAdjFlash();
            answer = SCPI::scpiAnswer[SCPI::ack];
        }
        else {
            answer = SCPI::scpiAnswer[SCPI::errexec];
        }
    }
    else {
        answer = SCPI::scpiAnswer[SCPI::nak];
    }
    return answer;
}

QString cClamp::handleScpiResetFlash(QString &scpiCmdStr)
{
    QString answer;
    cSCPICommand cmd = scpiCmdStr;
    if (cmd.isCommand(1) && (cmd.getParam(0) == "")) {
        if (resetAdjFlash()) {
            answer = SCPI::scpiAnswer[SCPI::ack];
        }
        else {
            answer = SCPI::scpiAnswer[SCPI::errexec];
        }
    }
    else {
        answer = SCPI::scpiAnswer[SCPI::nak];
    }
    return answer;
}

QString cClamp::handleScpiReadChksum(QString& scpiCmdStr)
{
    QString answer;
    cSCPICommand cmd = scpiCmdStr;
    if (cmd.isQuery()) {
        answer = QString("0x%1").arg(getChecksum(),0,16); // hex output
    }
    else {
        answer = SCPI::scpiAnswer[SCPI::nak];
    }
    return answer;
}

QString cClamp::handleScpiWriteXML(QString &scpiCmdStr)
{
    QString answer;
    cSCPICommand cmd = scpiCmdStr;
    if (cmd.isCommand(1)) {
        QString filename = cmd.getParam(0);
        if (exportAdjXML(filename)) {
            answer = SCPI::scpiAnswer[SCPI::ack];
        }
        else {
            answer = SCPI::scpiAnswer[SCPI::errexec];
        }
    }
    else {
        answer = SCPI::scpiAnswer[SCPI::nak];
    }
    return answer;
}

QString cClamp::handleScpiReadXML(QString &scpiCmdStr)
{
    QString answer;
    cSCPICommand cmd = scpiCmdStr;
    if (cmd.isCommand(1)) {
        QString filename = cmd.getParam(0);
        if (importAdjXML(filename)) {
            answer = SCPI::scpiAnswer[SCPI::ack];
        }
        else {
            answer = SCPI::scpiAnswer[SCPI::errexec];
        }
    }
    else {
        answer = SCPI::scpiAnswer[SCPI::nak];
    }
    return answer;
}

QString cClamp::handleScpiReadAdjStatus(QString &scpiCmdStr)
{
    QString answer;
    cSCPICommand cmd = scpiCmdStr;
    if (cmd.isQuery()) {
        answer = QString("%1").arg(getAdjustmentStatus()); // hex output
    }
    else {
        answer = SCPI::scpiAnswer[SCPI::nak];
    }
    return answer;
}
