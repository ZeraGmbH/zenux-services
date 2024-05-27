#include "adjdatacompleteinternstreamer.h"
#include "adjdatarangegroupstreamer.h"
#include <QDataStream>

AdjDataCompleteInternStreamer::AdjDataCompleteInternStreamer(int maxSize) :
    m_maxSize(maxSize),
    m_tmpWorkBuffer(new char[maxSize])
{
}

AdjDataCompleteInternStreamer::~AdjDataCompleteInternStreamer()
{
    delete[] m_tmpWorkBuffer;
}

bool AdjDataCompleteInternStreamer::decodeAdjBytes(QByteArray ba)
{
    qInfo("Decode adjustment data...");
    m_adjData = std::make_shared<AdjDataCompleteIntern>();
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

    decodeRanges(stream);

    qInfo("Decode succeeded.");
    m_isValid = true;
    return true;
}

bool AdjDataCompleteInternStreamer::isValid()
{
    return m_isValid;
}

std::shared_ptr<AdjDataCompleteIntern> AdjDataCompleteInternStreamer::getAdjData()
{
    return m_adjData;
}

bool AdjDataCompleteInternStreamer::decodeHeader(QDataStream &stream)
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

bool AdjDataCompleteInternStreamer::decodeServerVersion(QDataStream &stream)
{
    stream >> m_tmpWorkBuffer;
    if (QString(m_tmpWorkBuffer) != "ServerVersion") {
        qWarning("Adjustment data is missing ServerVersion tag!");
        return false;
    }
    stream >> m_tmpWorkBuffer;
    getAdjData()->getAdjHeader().m_serverVersion = m_tmpWorkBuffer;
    if(getAdjData()->getAdjHeader().m_serverVersion.isEmpty()) {
        qWarning("Adjustment data is missing server version!");
        return false;
    }
    return true;
}

bool AdjDataCompleteInternStreamer::decodeDeviceName(QDataStream &stream)
{
    stream >> m_tmpWorkBuffer;
    getAdjData()->getAdjHeader().m_deviceName = m_tmpWorkBuffer;
    if(getAdjData()->getAdjHeader().m_deviceName.isEmpty()) {
        qWarning("Adjustment data is missing device name!");
        return false;
    }
    return true;
}

bool AdjDataCompleteInternStreamer::decodeDeviceVersion(QDataStream &stream)
{
    stream >> m_tmpWorkBuffer;
    getAdjData()->getAdjHeader().m_deviceVersion = m_tmpWorkBuffer;
    if(getAdjData()->getAdjHeader().m_deviceVersion.isEmpty()) {
        qWarning("Adjustment data is missing device version!");
        return false;
    }
    return true;
}

bool AdjDataCompleteInternStreamer::decodeSerialNumber(QDataStream &stream)
{
    stream >> m_tmpWorkBuffer;
    getAdjData()->getAdjHeader().m_serialNumber = m_tmpWorkBuffer;
    if(getAdjData()->getAdjHeader().m_serialNumber.isEmpty()) {
        qWarning("Adjustment data is missing device serial number!");
        return false;
    }
    return true;
}

bool AdjDataCompleteInternStreamer::decodeAdjTimeStamp(QDataStream &stream)
{
    stream >> m_tmpWorkBuffer;
    QString adjTimeStamp = m_tmpWorkBuffer;
    if(adjTimeStamp.isEmpty()) {
        qWarning("Adjustment data is missing device adjustment timestamp!");
        return false;
    }
    getAdjData()->getAdjHeader().m_adjustmentDate = QDateTime::fromString(adjTimeStamp, Qt::TextDate);
    if(!getAdjData()->getAdjHeader().m_adjustmentDate.isValid()) {
        qWarning("Adjustment data'a adjustment timestamp is invalid!");
        return false;
    }
    return true;
}

void AdjDataCompleteInternStreamer::decodeRanges(QDataStream &stream)
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
                if(!m_adjData->isChannelRangeAvailable(channelName, rangeName)) {
                    std::shared_ptr<AdjDataRangeGroup> rangeAdjData = AdjDataRangeGroupStreamer::Deserialize(stream);
                    m_adjData->setChannelRange(channelName, rangeName, rangeAdjData);

                }
                else
                    qFatal("Channel %s / range %s was already added!", qPrintable(channelName), qPrintable(rangeName));
            }
        }
    }
}

