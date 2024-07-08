#ifndef CPUTEMP_H
#define CPUTEMP_H

#include "logvaluegetter.h"

#include <QString>

class CpuTemp : public LogValueGetter
{
public:
    CpuTemp();
    int getTemperature();
    float getValue();

private:
    QString findTempFileLocation();
};

#endif // CPUTEMP_H
