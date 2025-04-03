#ifndef MOCKSERVERPARAMGENERATOR_H
#define MOCKSERVERPARAMGENERATOR_H

#include "settingscontainer.h"

class MockServerParamGenerator
{
public:
    static ServerParams createParams(const QString &serviceNameForAlternateDevice);
};

#endif // MOCKSERVERPARAMGENERATOR_H
