#ifndef MOCKSERVERPARAMGENERATOR_H
#define MOCKSERVERPARAMGENERATOR_H

#include "settingscontainer.h"

class MockServerParamGenerator
{
public:
    static ServerParams createParams(const QString &nameForConfigAndRanges);
};

#endif // MOCKSERVERPARAMGENERATOR_H
