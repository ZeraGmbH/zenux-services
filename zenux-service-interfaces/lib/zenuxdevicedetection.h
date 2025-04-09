#ifndef ZENUXDEVICEDETECTION_H
#define ZENUXDEVICEDETECTION_H

#include <QString>

class ZenuxDeviceDetection
{
public:
    static QString getDeviceNameFromKernelParam();
};

#endif // ZENUXDEVICEDETECTION_H
