#ifndef ABSTRACTTESTALLSERVICES_H
#define ABSTRACTTESTALLSERVICES_H

#include "abstractallservices.h"
#include "zdspserver.h"

class AbstractTestAllServices  : public AbstractAllServices
{
public:
    virtual ~AbstractTestAllServices() = default;
    virtual ZDspServer *getZdsp1dServer() = 0;
    virtual void setRangeGetSetDelay(int rangeGetSetDelay) = 0;
};

#endif // ABSTRACTTESTALLSERVICES_H
