#ifndef CPUTEMP_H
#define CPUTEMP_H

#include "abstractlogvaluegetter.h"
#include <QString>

class CpuTemp : public AbstractLogValueGetter
{
public:
    bool canGetValue() override;
    float getValue() override;

private:
    static QString findTempFileLocation();
};

#endif // CPUTEMP_H
