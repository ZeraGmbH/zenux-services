#ifndef CPUFREQ_H
#define CPUFREQ_H

#include "abstractlogvaluegetter.h"

class CpuFreq : public AbstractLogValueGetter<float>
{
public:
    bool canGetValue() override;
    float getValue() override;
};

#endif // CPUFREQ_H
