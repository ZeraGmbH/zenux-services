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

    static QByteArray genJsonConstantValuesAllRanges(QList<SenseSystem::cChannelSettings*> channelSettings, Zera::cPCBInterface* pcbIFace);

    static void addRangeConstantDataToJson(QString rangeName, SenseSystem::cChannelSettings *channelSettings, QJsonObject &range);
    static bool compareRangeConstantDataWithJson(QJsonObject &rangeReference, QString clampName, QString rangeName, SenseSystem::cChannelSettings *channelSetting);
private:
    static void reportError(QString clampName, QString range, QString entry, QString expected, QString found);
};

#endif // SENSEREGRESSIONHELPER_H
