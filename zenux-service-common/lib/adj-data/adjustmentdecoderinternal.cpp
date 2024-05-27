#include "adjustmentdecoderinternal.h"
#include "adjustmentrangeserializer.h"
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

bool AdjustmentDecoderInternal::decodeAdjBytes(QByteArray ba)
{
    qInfo("Decode adjustment data...");
    m_adjData = std::make_shared<AdjustmentDataSet>();
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

bool AdjustmentDecoderInternal::isValid()
{
    return m_isValid;
}

std::shared_ptr<AdjustmentDataSet> AdjustmentDecoderInternal::getAdjData()
{
    return m_adjData;
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
    getAdjData()->getAdjHeader().m_serverVersion = m_tmpWorkBuffer;
    if(getAdjData()->getAdjHeader().m_serverVersion.isEmpty()) {
        qWarning("Adjustment data is missing server version!");
        return false;
    }
    return true;
}

bool AdjustmentDecoderInternal::decodeDeviceName(QDataStream &stream)
{
    stream >> m_tmpWorkBuffer;
    getAdjData()->getAdjHeader().m_deviceName = m_tmpWorkBuffer;
    if(getAdjData()->getAdjHeader().m_deviceName.isEmpty()) {
        qWarning("Adjustment data is missing device name!");
        return false;
    }
    return true;
}

bool AdjustmentDecoderInternal::decodeDeviceVersion(QDataStream &stream)
{
    stream >> m_tmpWorkBuffer;
    getAdjData()->getAdjHeader().m_deviceVersion = m_tmpWorkBuffer;
    if(getAdjData()->getAdjHeader().m_deviceVersion.isEmpty()) {
        qWarning("Adjustment data is missing device version!");
        return false;
    }
    return true;
}

bool AdjustmentDecoderInternal::decodeSerialNumber(QDataStream &stream)
{
    stream >> m_tmpWorkBuffer;
    getAdjData()->getAdjHeader().m_serialNumber = m_tmpWorkBuffer;
    if(getAdjData()->getAdjHeader().m_serialNumber.isEmpty()) {
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
    getAdjData()->getAdjHeader().m_adjustmentDate = QDateTime::fromString(adjTimeStamp, Qt::TextDate);
    if(!getAdjData()->getAdjHeader().m_adjustmentDate.isValid()) {
        qWarning("Adjustment data'a adjustment timestamp is invalid!");
        return false;
    }
    return true;
}

void AdjustmentDecoderInternal::decodeRanges(QDataStream &stream)
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
                    AdjustmentRangeSerializer adjRangeDecoder;
                    std::shared_ptr<AdjustmentDataRangeGroup> rangeAdjData = adjRangeDecoder.Deserialize(stream);
                    m_adjData->setChannelRange(channelName, rangeName, rangeAdjData);

                }
                else
                    qFatal("Channel %s / range %s was already added!", qPrintable(channelName), qPrintable(rangeName));
            }
        }
    }
}

