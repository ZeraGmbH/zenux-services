#ifndef ADJUSTMENTDECODERINTERNAL_H
#define ADJUSTMENTDECODERINTERNAL_H

#include "adjustmendataheader.h"
#include "adjustmentrangeserializer.h"
#include <QByteArray>
#include <QMap>


class AdjustmentDecoderInternal
{
public:
    struct rangeAdjStruct
    {
        AdjustmentDataSerializer gainSerializer;
        AdjustmentDataSerializer phaseSerializer;
        AdjustmentDataSerializer offsetSerializer;
    };

    AdjustmentDecoderInternal(int maxSize);
    ~AdjustmentDecoderInternal();

    void setDeviceName(QString deviceName);
    void setSerialNumber(QString serialNumber);

    bool decodeAdjBytes(QByteArray ba);
    bool isValid();
    const AdjustmentDataHeader& getAdjHeader();

    QMap<QString, QStringList> getRangeInfos();
    rangeAdjStruct getRangeAdjStruct(QString channelName, QString rangeName);
    bool isChannelRangeAvailable(QString channelName, QString rangeName);

private:
    bool decodeHeader(QDataStream &stream);
    bool decodeServerVersion(QDataStream &stream);
    bool decodeDeviceName(QDataStream &stream);
    bool decodeDeviceVersion(QDataStream &stream);
    bool decodeSerialNumber(QDataStream &stream);
    bool decodeAdjTimeStamp(QDataStream &stream);
    void extractRanges(QDataStream &stream);
    void setRangeAdjStruct(QString channelName, QString rangeName, AdjustmentRangeSerializer adjRangeDecoder);

    int m_maxSize;
    char* m_tmpWorkBuffer = nullptr;
    bool m_isValid = false;
    AdjustmentDataHeader m_adjHeader;

    QMap<QString, QStringList> m_rangeInfosMap;
    QMap<QString, QMap<QString, rangeAdjStruct>> m_rangeAdjMap;
    QString m_deviceName = "Unknown";
    QString m_serialNumber = "Unknown";
};

#endif // ADJUSTMENTDECODERINTERNAL_H
