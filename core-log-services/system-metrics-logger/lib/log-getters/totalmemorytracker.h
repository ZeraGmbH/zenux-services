#ifndef TOTALMEMORYTRACKER_H
#define TOTALMEMORYTRACKER_H

#include "abstractlogvaluegetter.h"

class TotalMemoryTracker : public AbstractLogValueGetter<float>
{
public:
    bool canGetValue() override;
    float getValue() override;

private:
    float calcPercentageOneDecimal(float value);
};

#endif // TOTALMEMORYTRACKER_H
