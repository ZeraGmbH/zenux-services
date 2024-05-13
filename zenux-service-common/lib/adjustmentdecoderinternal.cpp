#include "adjustmentdecoderinternal.h"
#include <QDataStream>

AdjustmentDecoderInternal::AdjustmentDecoderInternal(int maxSize) :
    m_maxSize(maxSize),
    m_tmpWorkBuffer(new char[maxSize])
{
}

AdjustmentDecoderInternal::~AdjustmentDecoderInternal()
{
    delete[] m_tmpWorkBuffer;
}

QMap<QString, QStringList> AdjustmentDecoderInternal::getRangeInfos()
{
    return m_rangeInfosMap;
}

bool AdjustmentDecoderInternal::isChannelRangeAvailable(QString channelName, QString rangeName)
{
    return m_rangeInfosMap.contains(channelName) && m_rangeInfosMap[channelName].contains(rangeName);
}

bool AdjustmentDecoderInternal::decodeAdjBytes(QByteArray ba)
{
    qInfo("Decode adjustment data...");
    if(ba.size() > m_maxSize) {
        qWarning("Adjustment data size exceeds max size: %i (max: %i)", ba.size(), m_maxSize);
        return false;
    }

    QDataStream stream(&ba, QIODevice::ReadOnly);
    stream.setVersion(QDataStream::Qt_5_4);

    quint32 sizeStored;
    stream >> sizeStored;
    if(sizeStored != ba.size()) {
        qWarning("Adjustment data size %i does not match size stored %i!", ba.size(), sizeStored);
        return false;
    }
    quint16 checkSum;
    stream >> checkSum; // ignore - checked in AdjustmentEepromReadWrite

    if(!decodeHeader(stream)) {
        qWarning("Adjustment data contains invalid header!");
        return false;
    }

    extractRanges(stream);

    qInfo("Decode succeeded.");
    m_isValid = true;
    return true;
}

bool AdjustmentDecoderInternal::isValid()
{
    return m_isValid;
}

const AdjustmentDataHeader &AdjustmentDecoderInternal::getAdjHeader()
{
    return m_adjHeader;
}

bool AdjustmentDecoderInternal::decodeHeader(QDataStream &stream)
{
    if(!decodeServerVersion(stream))
        return false;
    if(!decodeDeviceName(stream))
        return false;
    if(!decodeDeviceVersion(stream))
        return false;
    if(!decodeSerialNumber(stream))
        return false;
    // before 4.4.3 COM5003 did not store valid timestamps
    // see test_regression_adj_import_export_eeprom_com5003
    // * loadOriginalInvalidDateTimeRandomToEEpromWriteToFlashExportXmlAndCheck
    // * loadValidDateTimeRandomToEEpromWriteToFlashExportXmlAndCheck
    decodeAdjTimeStamp(stream);
    return true;
}

bool AdjustmentDecoderInternal::decodeServerVersion(QDataStream &stream)
{
    stream >> m_tmpWorkBuffer;
    if (QString(m_tmpWorkBuffer) != "ServerVersion") {
        qWarning("Adjustment data is missing ServerVersion tag!");
        return false;
    }
    stream >> m_tmpWorkBuffer;
    m_adjHeader.m_serverVersion = m_tmpWorkBuffer;
    if(m_adjHeader.m_serverVersion.isEmpty()) {
        qWarning("Adjustment data is missing server version!");
        return false;
    }
    return true;
}

bool AdjustmentDecoderInternal::decodeDeviceName(QDataStream &stream)
{
    stream >> m_tmpWorkBuffer;
    m_adjHeader.m_deviceName = m_tmpWorkBuffer;
    if(m_adjHeader.m_deviceName.isEmpty()) {
        qWarning("Adjustment data is missing device name!");
        return false;
    }
    return true;
}

bool AdjustmentDecoderInternal::decodeDeviceVersion(QDataStream &stream)
{
    stream >> m_tmpWorkBuffer;
    m_adjHeader.m_deviceVersion = m_tmpWorkBuffer;
    if(m_adjHeader.m_deviceVersion.isEmpty()) {
        qWarning("Adjustment data is missing device version!");
        return false;
    }
    return true;
}

bool AdjustmentDecoderInternal::decodeSerialNumber(QDataStream &stream)
{
    stream >> m_tmpWorkBuffer;
    m_adjHeader.m_serialNumber = m_tmpWorkBuffer;
    if(m_adjHeader.m_serialNumber.isEmpty()) {
        qWarning("Adjustment data is missing device serial number!");
        return false;
    }
    return true;
}

bool AdjustmentDecoderInternal::decodeAdjTimeStamp(QDataStream &stream)
{
    stream >> m_tmpWorkBuffer;
    QString adjTimeStamp = m_tmpWorkBuffer;
    if(adjTimeStamp.isEmpty()) {
        qWarning("Adjustment data is missing device adjustment timestamp!");
        return false;
    }
    m_adjHeader.m_adjustmentDate = QDateTime::fromString(adjTimeStamp, Qt::TextDate);
    if(!m_adjHeader.m_adjustmentDate.isValid()) {
        qWarning("Adjustment data'a adjustment timestamp is invalid!");
        return false;
    }
    return true;
}

void AdjustmentDecoderInternal::extractRanges(QDataStream &stream)
{
    QByteArray ba(m_maxSize, 0);
    stream.readRawData(ba.data(), ba.size());

    for(auto it = ba.cbegin(); it != ba.cend(); it++) {
        const QByteArray &data = it;
        if(data.startsWith("SENSE:")) {
            QList<QByteArray> rangeCmdList = data.split(':'); //[0]:SENSE  [1]:m0  [2]:250V
            QByteArray channelName = rangeCmdList[1];
            QByteArray rangeName = rangeCmdList[2];
            if(!m_rangeInfosMap[channelName].contains(rangeName))
                m_rangeInfosMap[rangeCmdList[1]].append(rangeCmdList[2]);
            else
                qFatal("Channel %s / range %s was already added!", qPrintable(channelName), qPrintable(rangeName));
        }
    }
    for(auto it = m_rangeInfosMap.begin(); it != m_rangeInfosMap.end(); it++) {
        QStringList ranges = it.value();
        for(int i = 0; i < ranges.size(); i++)
            qInfo("Channel %s / Range %s", qPrintable(it.key()), qPrintable(ranges.at(i)));
    }
}

