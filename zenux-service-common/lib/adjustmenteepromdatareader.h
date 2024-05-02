#ifndef ADJUSTMENTEEPROMDATAREADER_H
#define ADJUSTMENTEEPROMDATAREADER_H

#include <QObject>
#include <QMap>

class AdjustmentEepromDataReader
{
public:
    AdjustmentEepromDataReader(QDataStream &stream);
    AdjustmentEepromDataReader();
    bool extractDeviceInfos(QDataStream &stream);
    bool ignoreCountAndCheckSum(QDataStream &stream);

    QString getDeviceName();
    QString getServerVersion();
    QMap<QString, QStringList> getRangeInfos();

private:
    bool extractServerVersion(QDataStream &stream, char* s);
    void extractDeviceName(QDataStream &stream, char* s);
    void IgnoreUselessInfos(QDataStream &stream, char* s);
    void extractRanges(QDataStream &stream);

    QString m_deviceName;
    QString m_serverVersion;
    QMap<QString, QStringList> m_rangeInfosMap;
};

#endif // ADJUSTMENTEEPROMDATAREADER_H
