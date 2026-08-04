#ifndef SENSEREGRESSIONHELPER_H
#define SENSEREGRESSIONHELPER_H

#include "sensesettings.h"
#include "pcbinterface.h"
#include <QJsonObject>
#include <QList>

class SenseRegressionHelper
{
public:
    static QString getJsonNumString(int clampTypeNo);

    static QByteArray genJsonConstantValuesAllRanges(const QList<SenseSystem::cChannelSettings*> &channelSettings,
                                                     Zera::cPCBInterface* pcbIFace);

    static void addRangeConstantDataToJson(const QString &rangeName,
                                           const SenseSystem::cChannelSettings *channelSettings,
                                           QJsonObject &range);
private:
    static void reportError(const QString &clampName,
                            const QString &range,
                            const QString &entry,
                            const QString &expected,
                            const QString &found);
};

#endif // SENSEREGRESSIONHELPER_H
