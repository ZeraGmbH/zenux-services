#include "diskiototalcalculator.h"

DiskIoTotalCalculator::DiskIoTotalCalculator(const QStringList &diskBlockDeviceNames)
{
    for(const QString& devName : diskBlockDeviceNames)
        m_diskCalculators.append(DiskIoCalculator(devName));
}

DiskValues DiskIoTotalCalculator::getReadWriteSinceLast()
{
    DiskValues totalValuesSince;
    for(DiskIoCalculator &calc : m_diskCalculators) {
        DiskValues valuesSince = calc.getReadWriteSinceLast();
        totalValuesSince.totalReadBytes += valuesSince.totalReadBytes;
        totalValuesSince.totalWriteBytes += valuesSince.totalWriteBytes;
    }
    return totalValuesSince;
}
