#ifndef REGRESSIONHELPER_H
#define REGRESSIONHELPER_H

#include "sensesettings.h"
#include "pcbinterface.h"

class RegressionHelper
{
public:
    static QString getJsonNumString(int clampTypeNo);
    static void addRangeConstantDataToJson(QString rangeName, SenseSystem::cChannelSettings *channelSettings, QJsonObject &range);
    static bool compareRangeConstantDataWithJson(QJsonObject &rangeReference, QString clampName, QString rangeName, SenseSystem::cChannelSettings *channelSettings);
    static QString noClampJsonId;
    static void genJsonConstantValuesAllRanges(SenseSystem::cChannelSettings *channelSetting, Zera::cPCBInterface* pcbIFace);
private:
    static void reportError(QString clampName, QString range, QString entry, QString expected, QString found);
};

#endif // REGRESSIONHELPER_H
