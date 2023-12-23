#ifndef REGRESSIONHELPER_H
#define REGRESSIONHELPER_H

#include "sensesettings.h"

class RegressionHelper
{
public:
    static void addRangeConstantDataToJson(QString rangeName, SenseSystem::cChannelSettings *channelSettings, QJsonObject &range);
};

#endif // REGRESSIONHELPER_H
