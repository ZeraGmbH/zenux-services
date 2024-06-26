#ifndef ADJDATACOMPLETEINTERNSTREAM_H
#define ADJDATACOMPLETEINTERNSTREAM_H

#include "adjdataheaderintern.h"
#include "adjdatacompleteintern.h"
#include <QByteArray>
#include <QMap>
#include <memory>

typedef std::shared_ptr<AdjDataCompleteIntern> AdjDataPtr;

class AdjDataCompleteInternStream
{
public:
    AdjDataCompleteInternStream(int maxSize);
    ~AdjDataCompleteInternStream();

    AdjDataPtr decodeAdjBytes(QByteArray ba);
private:
    bool decodeHeader(QDataStream &stream, AdjDataPtr adjData);
    bool decodeServerVersion(QDataStream &stream, AdjDataPtr adjData);
    bool decodeDeviceName(QDataStream &stream, AdjDataPtr adjData);
    bool decodeDeviceVersion(QDataStream &stream, AdjDataPtr adjData);
    bool decodeSerialNumber(QDataStream &stream, AdjDataPtr adjData);
    bool decodeAdjTimeStamp(QDataStream &stream, AdjDataPtr adjData);
    void decodeRanges(QDataStream &stream, AdjDataPtr adjData);
    
    int m_maxSize;
    char* m_tmpWorkBuffer = nullptr;
    bool m_isValid = false;
};

#endif // ADJDATACOMPLETEINTERNSTREAM_H
