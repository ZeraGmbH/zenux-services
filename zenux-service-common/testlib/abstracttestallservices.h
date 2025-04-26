#ifndef ABSTRACTTESTALLSERVICES_H
#define ABSTRACTTESTALLSERVICES_H

#include "zdspserver.h"

class AbstractTestAllServices
{
public:
    virtual ~AbstractTestAllServices() = default;
    virtual ZDspServer *getZdsp1dServer() = 0;
};

#endif // ABSTRACTTESTALLSERVICES_H
