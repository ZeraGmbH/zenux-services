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

    bool extractDeviceInfos(QByteArray ba);

    QString getDeviceName();
    QString getServerVersion();
    QMap<QString, QStringList> getRangeInfos();
    bool isChannelRangeAvailable(QString channelName, QString rangeName);

private:
    bool ignoreCountAndCheckSum(QDataStream &stream);
    bool extractServerVersion(QDataStream &stream);
    void extractDeviceName(QDataStream &stream);
    void IgnoreUselessInfos(QDataStream &stream);
    void extractRanges(QDataStream &stream);

    quint32 m_maxSize;
    char* m_tmpWorkBuffer = nullptr;
    AdjustmentDataHeader m_adjHeader;

    QMap<QString, QStringList> m_rangeInfosMap;
};

#endif // ADJUSTMENTDECODERINTERNAL_H
