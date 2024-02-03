#ifndef MOCKSERVERPARAMGENERATOR_H
#define MOCKSERVERPARAMGENERATOR_H

#include "settingscontainer.h"

class MockServerParamGenerator
{
public:
    static ServerParams createParams(QString serviceName);
};

#endif // MOCKSERVERPARAMGENERATOR_H
