#ifndef CPUTEMP_H
#define CPUTEMP_H

#include "abstractlogvaluegetter.h"
#include <QString>

class CpuTemp : public AbstractLogValueGetter
{
public:
    int getTemperature();
    float getValue();

private:
    QString findTempFileLocation();
};

#endif // CPUTEMP_H
