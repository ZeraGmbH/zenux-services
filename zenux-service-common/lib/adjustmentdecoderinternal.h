#ifndef ADJUSTMENTDECODERINTERNAL_H
#define ADJUSTMENTDECODERINTERNAL_H

#include <QObject>
#include <QMap>

class AdjustmentDecoderInternal
{
public:
    AdjustmentDecoderInternal(quint32 maxSize);
    bool extractDeviceInfos(QDataStream &stream);
    bool ignoreCountAndCheckSum(QDataStream &stream);

    QString getDeviceName();
    QString getServerVersion();
    QMap<QString, QStringList> getRangeInfos();
    bool isChannelRangeAvailable(QString channelName, QString rangeName);

private:
    bool extractServerVersion(QDataStream &stream, char* s);
    void extractDeviceName(QDataStream &stream, char* s);
    void IgnoreUselessInfos(QDataStream &stream, char* s);
    void extractRanges(QDataStream &stream);

    quint32 m_maxSize;
    QString m_deviceName;
    QString m_serverVersion;

    QMap<QString, QStringList> m_rangeInfosMap;
};

#endif // ADJUSTMENTDECODERINTERNAL_H
