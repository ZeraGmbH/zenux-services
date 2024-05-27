#ifndef ADJDATACOMPLETEINTERNSTREAMER_H
#define ADJDATACOMPLETEINTERNSTREAMER_H

#include "adjdataheaderintern.h"
#include "adjdatacompleteintern.h"
#include <QByteArray>
#include <QMap>
#include <memory>

class AdjDataCompleteInternStreamer
{
public:
    AdjDataCompleteInternStreamer(int maxSize);
    ~AdjDataCompleteInternStreamer();

    bool decodeAdjBytes(QByteArray ba);
    bool isValid();
    
    std::shared_ptr<AdjDataCompleteIntern> getAdjData();

private:
    bool decodeHeader(QDataStream &stream);
    bool decodeServerVersion(QDataStream &stream);
    bool decodeDeviceName(QDataStream &stream);
    bool decodeDeviceVersion(QDataStream &stream);
    bool decodeSerialNumber(QDataStream &stream);
    bool decodeAdjTimeStamp(QDataStream &stream);
    void decodeRanges(QDataStream &stream);
    
    std::shared_ptr<AdjDataCompleteIntern> m_adjData;
    int m_maxSize;
    char* m_tmpWorkBuffer = nullptr;
    bool m_isValid = false;
};

#endif // ADJDATACOMPLETEINTERNSTREAMER_H
