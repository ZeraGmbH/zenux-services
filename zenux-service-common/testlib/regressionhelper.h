#ifndef REGRESSIONHELPER_H
#define REGRESSIONHELPER_H

#include "sensesettings.h"

class RegressionHelper
{
public:
    static void addRangeConstantDataToJson(QString rangeName, SenseSystem::cChannelSettings *channelSettings, QJsonObject &range);
    static bool compareRangeConstantDataWithJson(QJsonObject &rangeReference, QString clampName, QString rangeName, SenseSystem::cChannelSettings *channelSettings);
private:
    static void reportError(QString clampName, QString range, QString entry, QString expected, QString found);
};

#endif // REGRESSIONHELPER_H