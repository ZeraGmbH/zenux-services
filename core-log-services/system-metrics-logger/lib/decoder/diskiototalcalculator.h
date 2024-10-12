#ifndef DISKIOTOTALCALCULATOR_H
#define DISKIOTOTALCALCULATOR_H

#include "diskiocalculator.h"
#include <QStringList>

class DiskIoTotalCalculator
{
public:
    DiskIoTotalCalculator(const QStringList &diskBlockDeviceNames);
    DiskValues getReadWriteSinceLast();
private:
    QList<DiskIoCalculator> m_diskCalculators;
};

#endif // DISKIOTOTALCALCULATOR_H
