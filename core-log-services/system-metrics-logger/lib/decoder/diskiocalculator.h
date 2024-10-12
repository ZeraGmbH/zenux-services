#ifndef DISKIOCALCULATOR_H
#define DISKIOCALCULATOR_H

#include "diskreadwritevalues.h"
#include <QString>

class DiskIoCalculator
{
public:
    DiskIoCalculator(const QString& diskBlockDevName);
    DiskValues getReadWriteSinceLast();
private:
    QString m_diskBlockDevName;
    DiskValues m_currentValues;
};

#endif // DISKIOCALCULATOR_H
