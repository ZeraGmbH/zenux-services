#ifndef ADJUSTMENTDECODERINTERNAL_H
#define ADJUSTMENTDECODERINTERNAL_H

#include "adjustmendataheader.h"
#include "adjustmentdata.h"
#include "adjustmentdata.h"
#include <QByteArray>
#include <QMap>
#include <memory>

class AdjustmentDecoderInternal
{
public:
    AdjustmentDecoderInternal(int maxSize);
    ~AdjustmentDecoderInternal();

    bool decodeAdjBytes(QByteArray ba);
    //QByteArray encodeAdjData(std::shared_ptr<AdjustmentRangeData> adjData);
    bool isValid();

    std::shared_ptr<AdjustmentData> getAdjData();











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

    std::shared_ptr<AdjustmentData> m_adjData;
    int m_maxSize;
    char* m_tmpWorkBuffer = nullptr;
    bool m_isValid = false;

    QMap<QString, QStringList> m_rangeInfosMap;
};

#endif // ADJUSTMENTDECODERINTERNAL_H
