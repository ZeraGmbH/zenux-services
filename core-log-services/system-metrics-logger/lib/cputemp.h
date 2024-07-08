#ifndef CPUTEMP_H
#define CPUTEMP_H

#include "abstractlogvaluegetter.h"
#include <QString>

class CpuTemp : public AbstractLogValueGetter
{
public:
    float getValue() override;

private:
    static QString findTempFileLocation();
};

#endif // CPUTEMP_H
