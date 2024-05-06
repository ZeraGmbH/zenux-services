#include "adjustmenteepromdatareader.h"
#include <QDataStream>

AdjustmentEepromDataReader::AdjustmentEepromDataReader(QDataStream &stream)
{
    extractDeviceInfos(stream);
}

AdjustmentEepromDataReader::AdjustmentEepromDataReader()
{
}

QString AdjustmentEepromDataReader::getDeviceName()
{
    return m_deviceName;
}

QString AdjustmentEepromDataReader::getServerVersion()
{
    return m_serverVersion;
}

QMap<QString, QStringList> AdjustmentEepromDataReader::getRangeInfos()
{
    return m_rangeInfosMap;
}

bool AdjustmentEepromDataReader::isChannelRangeAvailable(QString channelName, QString rangeName)
{
    return m_rangeInfosMap.contains(channelName) && m_rangeInfosMap[channelName].contains(rangeName);
}

bool AdjustmentEepromDataReader::ignoreCountAndCheckSum(QDataStream &stream)
{
    // we need count and chksum only to check if file is not empty
    if(stream.skipRawData(6) == -1) {
        qCritical("Empty file ?");
        return false;
    }
    return true;
}

bool AdjustmentEepromDataReader::extractServerVersion(QDataStream &stream, char *s)
{
    stream >> s;
    if (QString(s) != "ServerVersion") {
        qCritical("Flashmemory read: ServerVersion not found");
        return false;
    }
    else {
        stream >> s;
        m_serverVersion = QString(s);
        return true;
    }
}

void AdjustmentEepromDataReader::extractDeviceName(QDataStream &stream, char *s)
{
    stream >> s; // device name
    m_deviceName = QString(s);
}

void AdjustmentEepromDataReader::IgnoreUselessInfos(QDataStream &stream, char *s)
{
    stream >> s; // device version
    stream >> s; // serial number
    stream >> s; // date & time
}

void AdjustmentEepromDataReader::extractRanges(QDataStream &stream)
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

bool AdjustmentEepromDataReader::extractDeviceInfos(QDataStream &stream)
{
    char flashdata[200];
    char* s = flashdata;

    if(!ignoreCountAndCheckSum(stream))
        return false;

    if(!extractServerVersion(stream, s))
        return false;
    extractDeviceName(stream, s);
    IgnoreUselessInfos(stream, s);
    extractRanges(stream);

    return true;
}
