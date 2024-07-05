#ifndef CPUTEMP_H
#define CPUTEMP_H

#include <QString>

class CpuTemp
{
public:
    CpuTemp();
    int getTemperature();

private:
    QString findTempFileLocation();
};

#endif // CPUTEMP_H
