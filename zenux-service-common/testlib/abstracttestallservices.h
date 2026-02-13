#ifndef ABSTRACTTESTALLSERVICES_H
#define ABSTRACTTESTALLSERVICES_H

#include "abstractmockallservices.h"
#include "sec1000d.h"
#include "zdspserver.h"

class AbstractTestAllServices : public AbstractMockAllServices
{
public:
    virtual ~AbstractTestAllServices() = default;
    virtual ZDspServer *getZdsp1dServer() = 0;
    virtual cSEC1000dServer *getSecServer() = 0;
    virtual void setRangeGetSetDelay(int rangeGetSetDelay) = 0;
};

#endif // ABSTRACTTESTALLSERVICES_H
