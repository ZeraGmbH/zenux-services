#include "adjdatacompleteinternstream.h"
#include <QIODevice>
#include <QDataStream>

AdjDataCompleteInternStream::AdjDataCompleteInternStream(int maxSize) :
    m_maxSize(maxSize),
    m_tmpWorkBuffer(new char[maxSize])
{
}

AdjDataCompleteInternStream::~AdjDataCompleteInternStream()
{
    delete[] m_tmpWorkBuffer;
}

AdjDataPtr AdjDataCompleteInternStream::decodeAdjBytes(QByteArray ba)
{
    qInfo("Decode adjustment data...");
    std::shared_ptr<AdjDataCompleteIntern> adjData = std::make_shared<AdjDataCompleteIntern>();
    if(ba.size() > m_maxSize) {
        qWarning("Adjustment data size exceeds max size: %i (max: %i)", ba.size(), m_maxSize);
        return adjData;
    }

    QDataStream stream(&ba, QIODevice::ReadOnly);
    stream.setVersion(QDataStream::Qt_5_4);

    quint32 sizeStored;
    stream >> sizeStored;
    if(sizeStored != ba.size()) {
        qWarning("Adjustment data size %i does not match size stored %i!", ba.size(), sizeStored);
        return adjData;
    }
    quint16 checkSum;
    stream >> checkSum; // ignore - checked in AdjustmentEepromReadWrite

    if(!decodeHeader(stream, adjData)) {
        qWarning("Adjustment data contains invalid header!");
        return adjData;
    }

    decodeRanges(stream, adjData);

    qInfo("Decode succeeded.");
    m_isValid = true;
    return adjData;
}

bool AdjDataCompleteInternStream::decodeHeader(QDataStream &stream, AdjDataPtr adjData)
{
    if(!decodeServerVersion(stream, adjData))
        return false;
    if(!decodeDeviceName(stream, adjData))
        return false;
    if(!decodeDeviceVersion(stream, adjData))
        return false;
    if(!decodeSerialNumber(stream, adjData))
        return false;
    // before 4.4.3 COM5003 did not store valid timestamps
    // see test_regression_adj_import_export_eeprom_com5003
    // * loadOriginalInvalidDateTimeRandomToEEpromWriteToFlashExportXmlAndCheck
    // * loadValidDateTimeRandomToEEpromWriteToFlashExportXmlAndCheck
    decodeAdjTimeStamp(stream, adjData);
    return true;
}

bool AdjDataCompleteInternStream::decodeServerVersion(QDataStream &stream, AdjDataPtr adjData)
{
    stream >> m_tmpWorkBuffer;
    if (QString(m_tmpWorkBuffer) != "ServerVersion") {
        qWarning("Adjustment data is missing ServerVersion tag!");
        return false;
    }
    stream >> m_tmpWorkBuffer;
    adjData->getAdjHeader().m_serverVersion = m_tmpWorkBuffer;
    if(adjData->getAdjHeader().m_serverVersion.isEmpty()) {
        qWarning("Adjustment data is missing server version!");
        return false;
    }
    return true;
}

bool AdjDataCompleteInternStream::decodeDeviceName(QDataStream &stream, AdjDataPtr adjData)
{
    stream >> m_tmpWorkBuffer;
    adjData->getAdjHeader().m_deviceName = m_tmpWorkBuffer;
    if(adjData->getAdjHeader().m_deviceName.isEmpty()) {
        qWarning("Adjustment data is missing device name!");
        return false;
    }
    return true;
}

bool AdjDataCompleteInternStream::decodeDeviceVersion(QDataStream &stream, AdjDataPtr adjData)
{
    stream >> m_tmpWorkBuffer;
    adjData->getAdjHeader().m_deviceVersion = m_tmpWorkBuffer;
    if(adjData->getAdjHeader().m_deviceVersion.isEmpty()) {
        qWarning("Adjustment data is missing device version!");
        return false;
    }
    return true;
}

bool AdjDataCompleteInternStream::decodeSerialNumber(QDataStream &stream, AdjDataPtr adjData)
{
    stream >> m_tmpWorkBuffer;
    adjData->getAdjHeader().m_serialNumber = m_tmpWorkBuffer;
    if(adjData->getAdjHeader().m_serialNumber.isEmpty()) {
        qWarning("Adjustment data is missing device serial number!");
        return false;
    }
    return true;
}

bool AdjDataCompleteInternStream::decodeAdjTimeStamp(QDataStream &stream, AdjDataPtr adjData)
{
    stream >> m_tmpWorkBuffer;
    QString adjTimeStamp = m_tmpWorkBuffer;
    if(adjTimeStamp.isEmpty()) {
        qWarning("Adjustment data is missing device adjustment timestamp!");
        return false;
    }
    adjData->getAdjHeader().m_adjustmentDate = QDateTime::fromString(adjTimeStamp, Qt::TextDate);
    if(!adjData->getAdjHeader().m_adjustmentDate.isValid()) {
        qWarning("Adjustment data'a adjustment timestamp is invalid!");
        return false;
    }
    return true;
}

void AdjDataCompleteInternStream::decodeRanges(QDataStream &stream, AdjDataPtr adjData)
{
    while (!stream.atEnd()) {
        QString channelRangeInfo;
        stream >> m_tmpWorkBuffer;
        channelRangeInfo = m_tmpWorkBuffer;
        if(!channelRangeInfo.isEmpty()) {
            QStringList rangeCmdList = channelRangeInfo.split(':'); //[0]:SENSE  [1]:m0  [2]:250V
            QString senseId = rangeCmdList[0];
            if(senseId == "SENSE") {
                QString channelName = rangeCmdList[1];
                QString rangeName = rangeCmdList[2];
                if(!adjData->isChannelRangeAvailable(channelName, rangeName)) {
                    AdjDataRange rangeAdjData;
                    rangeAdjData.fromStream(stream);
                    adjData->setChannelRange(channelName, rangeName, rangeAdjData);
                }
                else
                    qFatal("Channel %s / range %s was already added!", qPrintable(channelName), qPrintable(rangeName));
            }
        }
    }
}

