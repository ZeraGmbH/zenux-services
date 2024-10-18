#ifndef PROCESSCOUNT_H
#define PROCESSCOUNT_H

#include "abstractlogvaluegetter.h"

class ProcessCount : public AbstractLogValueGetter<float>
{
public:
    bool canGetValue() override;
    float getValue() override;
};

#endif // PROCESSCOUNT_H
