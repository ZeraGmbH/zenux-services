#ifndef ADJUSTMENTDECODERINTERNAL_H
#define ADJUSTMENTDECODERINTERNAL_H

#include "adjustmendataheader.h"
#include <QByteArray>
#include <QMap>

class AdjustmentDecoderInternal
{
public:
    AdjustmentDecoderInternal(quint32 maxSize);
    ~AdjustmentDecoderInternal();

    bool decodeAdjBytes(QByteArray ba);
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

    quint32 m_maxSize;
    char* m_tmpWorkBuffer = nullptr;
    AdjustmentDataHeader m_adjHeader;

    QMap<QString, QStringList> m_rangeInfosMap;
};

#endif // ADJUSTMENTDECODERINTERNAL_H
