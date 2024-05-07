#include "adjustmentdecoderinternal.h"
#include <QDataStream>

AdjustmentDecoderInternal::AdjustmentDecoderInternal(quint32 maxSize) :
    m_maxSize(maxSize),
    m_tmpWorkBuffer(new char[maxSize])
{
}

AdjustmentDecoderInternal::~AdjustmentDecoderInternal()
{
    delete[] m_tmpWorkBuffer;
}

QString AdjustmentDecoderInternal::getDeviceName()
{
    return m_adjHeader.m_deviceName;
}

QString AdjustmentDecoderInternal::getServerVersion()
{
    return m_adjHeader.m_serverVersion;
}

QMap<QString, QStringList> AdjustmentDecoderInternal::getRangeInfos()
{
    return m_rangeInfosMap;
}

bool AdjustmentDecoderInternal::isChannelRangeAvailable(QString channelName, QString rangeName)
{
    return m_rangeInfosMap.contains(channelName) && m_rangeInfosMap[channelName].contains(rangeName);
}

bool AdjustmentDecoderInternal::extractDeviceInfos(QByteArray ba)
{
    if(ba.size() > m_maxSize) {
        qWarning("Adjustment data size exceeds max size: %i (max: %i)", ba.size(), m_maxSize);
        return false;
    }

    QDataStream stream(&ba, QIODevice::ReadOnly);
    stream.setVersion(QDataStream::Qt_5_4);

    if(!ignoreCountAndCheckSum(stream))
        return false;

    if(!extractServerVersion(stream))
        return false;
    extractDeviceName(stream);
    IgnoreUselessInfos(stream);
    extractRanges(stream);

    return true;
}

bool AdjustmentDecoderInternal::ignoreCountAndCheckSum(QDataStream &stream)
{
    // we need count and chksum only to check if file is not empty
    if(stream.skipRawData(6) == -1) {
        qCritical("Empty file ?");
        return false;
    }
    return true;
}

bool AdjustmentDecoderInternal::extractServerVersion(QDataStream &stream)
{
    stream >> m_tmpWorkBuffer;
    if (QString(m_tmpWorkBuffer) != "ServerVersion") {
        qCritical("Flashmemory read: ServerVersion not found");
        return false;
    }
    else {
        stream >> m_tmpWorkBuffer;
        m_adjHeader.m_serverVersion = QString(m_tmpWorkBuffer);
        return true;
    }
}

void AdjustmentDecoderInternal::extractDeviceName(QDataStream &stream)
{
    stream >> m_tmpWorkBuffer; // device name
    m_adjHeader.m_deviceName = QString(m_tmpWorkBuffer);
}

void AdjustmentDecoderInternal::IgnoreUselessInfos(QDataStream &stream)
{
    stream >> m_tmpWorkBuffer; // device version
    stream >> m_tmpWorkBuffer; // serial number
    stream >> m_tmpWorkBuffer; // date & time
}

void AdjustmentDecoderInternal::extractRanges(QDataStream &stream)
{
    QByteArray ba(100000, 0);
    stream.readRawData(ba.data(), ba.size());

    QByteArray::iterator it;
    for(it = ba.begin(); it != ba.end(); it++) {
        QString data = QString(it);
        if(data.startsWith("SENSE")) {
            QStringList rangeCmdList = data.split(':'); //[0]:SENSE  [1]:m0  [2]:250V
            if(m_rangeInfosMap.contains(rangeCmdList[1])) {
                QStringList values = m_rangeInfosMap.value(rangeCmdList[1]);
                values.append(rangeCmdList[2]);
                m_rangeInfosMap.insert(rangeCmdList[1], values);
            }
            else {
                QStringList values;
                values.append(rangeCmdList[2]);
                m_rangeInfosMap.insert(rangeCmdList[1], values);
            }
        }
    }
}

