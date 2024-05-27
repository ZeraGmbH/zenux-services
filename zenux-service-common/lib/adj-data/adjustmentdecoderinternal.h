#ifndef ADJUSTMENTDECODERINTERNAL_H
#define ADJUSTMENTDECODERINTERNAL_H

#include "adjustmendataheader.h"
#include "adjustmentdataset.h"
#include <QByteArray>
#include <QMap>
#include <memory>

class AdjustmentDecoderInternal
{
public:
    AdjustmentDecoderInternal(int maxSize);
    ~AdjustmentDecoderInternal();

    bool decodeAdjBytes(QByteArray ba);
    bool isValid();

    std::shared_ptr<AdjustmentDataSet> getAdjData();

private:
    bool decodeHeader(QDataStream &stream);
    bool decodeServerVersion(QDataStream &stream);
    bool decodeDeviceName(QDataStream &stream);
    bool decodeDeviceVersion(QDataStream &stream);
    bool decodeSerialNumber(QDataStream &stream);
    bool decodeAdjTimeStamp(QDataStream &stream);
    void decodeRanges(QDataStream &stream);

    std::shared_ptr<AdjustmentDataSet> m_adjData;
    int m_maxSize;
    char* m_tmpWorkBuffer = nullptr;
    bool m_isValid = false;
};

#endif // ADJUSTMENTDECODERINTERNAL_H
