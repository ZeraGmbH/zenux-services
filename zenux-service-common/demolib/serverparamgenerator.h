#ifndef SERVERPARAMGENERATOR_H
#define SERVERPARAMGENERATOR_H

#include "pcbserver.h"

class ServerParamGenerator
{
public:
    static ServerParams createParams(QString serviceName);
};

#endif // SERVERPARAMGENERATOR_H
