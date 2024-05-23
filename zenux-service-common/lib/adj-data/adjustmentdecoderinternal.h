#ifndef ADJUSTMENTDECODERINTERNAL_H
#define ADJUSTMENTDECODERINTERNAL_H

#include "adjustmendataheader.h"
#include "adjustmentrangeserializer.h"
#include "rangeadjinterface.h"
#include <QByteArray>
#include <QMap>

class AdjustmentDecoderInternal
{
public:
    struct rangeAdjStruct
    {
        AdjustmentDataSerializer gainSerializer = GainCorrOrder;
        AdjustmentDataSerializer phaseSerializer = PhaseCorrOrder;
        AdjustmentDataSerializer offsetSerializer = OffsetCorrOrder;
    };

    AdjustmentDecoderInternal(int maxSize);
    ~AdjustmentDecoderInternal();

    bool decodeAdjBytes(QByteArray ba);
    bool isValid();
    bool isChannelRangeAvailable(QString channelName, QString rangeName);
    void setRangeAdjStruct(QString channelName, QString rangeName, AdjustmentRangeSerializer adjRangeDecoder);

    const AdjustmentDataHeader& getAdjHeader();
    QMap<QString, QStringList> getRangeInfos();
    rangeAdjStruct getRangeAdjStruct(QString channelName, QString rangeName);
    QString getDeviceName();
    QString getSerialNumber();

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
    QMap<QString, QMap<QString, rangeAdjStruct>> m_rangeAdjMap;
    QMap<QString, rangeAdjStruct> m_adjInterface;

};

#endif // ADJUSTMENTDECODERINTERNAL_H
