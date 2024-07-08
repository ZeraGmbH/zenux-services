#ifndef CPUTEMP_H
#define CPUTEMP_H

#include "logvaluegetter.h"
#include <QString>

class CpuTemp : public LogValueGetter
{
public:
    int getTemperature();
    float getValue();

private:
    QString findTempFileLocation();
};

#endif // CPUTEMP_H
