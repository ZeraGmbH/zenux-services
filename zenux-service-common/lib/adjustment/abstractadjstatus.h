#ifndef ABSTRACTADJSTATUS_H
#define ABSTRACTADJSTATUS_H

#include <QtGlobal>

class AbstractAdjStatus
{
public:
    virtual ~AbstractAdjStatus() = default;
    virtual quint8 getAdjustmentStatus() = 0;
};

#endif // ABSTRACTADJSTATUS_H
