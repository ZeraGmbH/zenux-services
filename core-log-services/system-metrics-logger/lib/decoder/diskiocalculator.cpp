#include "diskiocalculator.h"
#include "procdiskstatdecoder.h"

DiskIoCalculator::DiskIoCalculator(const QString &diskBlockDevName) :
    m_diskBlockDevName(diskBlockDevName)
{
}

DiskValues DiskIoCalculator::getReadWriteSinceLast()
{
    DiskValues nextValues = ProcDiskStatDecoder::getReadWriteBytes(m_diskBlockDevName);
    DiskValues valueSince;
    valueSince.totalReadBytes = nextValues.totalReadBytes - m_currentValues.totalReadBytes;
    valueSince.totalWriteBytes = nextValues.totalWriteBytes - m_currentValues.totalWriteBytes;
    m_currentValues = nextValues;
    return valueSince;
}
