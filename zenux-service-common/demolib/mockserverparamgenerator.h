#ifndef MOCKSERVERPARAMGENERATOR_H
#define MOCKSERVERPARAMGENERATOR_H

#include "settingscontainer.h"

class MockServerParamGenerator
{
public:
    static ServerParams createParams(const QString &serviceName);
};

#endif // MOCKSERVERPARAMGENERATOR_H
