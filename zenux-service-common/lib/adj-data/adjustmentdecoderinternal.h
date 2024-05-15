#ifndef ADJUSTMENTDECODERINTERNAL_H
#define ADJUSTMENTDECODERINTERNAL_H

#include "adjustmendataheader.h"
#include <QByteArray>
#include <QMap>

class AdjustmentDecoderInternal
{
public:
    AdjustmentDecoderInternal(int maxSize);
    ~AdjustmentDecoderInternal();

    void setDeviceName(QString deviceName);
    void setSerialNumber(QString serialNumber);

    bool decodeAdjBytes(QByteArray ba);
    bool isValid();
    const AdjustmentDataHeader& getAdjHeader();

    QMap<QString, QStringList> getRangeInfos();
    bool isChannelRangeAvailable(QString channelName, QString rangeName);

private:
    bool decodeHeader(QDataStream &stream);
    bool decodeServerVersion(QDataStream &stream);
    bool decodeDeviceName(QDataStream &stream);
    bool decodeDeviceVersion(QDataStream &stream);
    bool decodeSerialNumber(QDataStream &stream);
    bool decodeAdjTimeStamp(QDataStream &stream);
    void extractRanges(QDataStream &stream);

    int m_maxSize;
    char* m_tmpWorkBuffer = nullptr;
    bool m_isValid = false;
    AdjustmentDataHeader m_adjHeader;

    QMap<QString, QStringList> m_rangeInfosMap;
    QString m_deviceName = "Unknown";
    QString m_serialNumber = "Unknown";
};

#endif // ADJUSTMENTDECODERINTERNAL_H
