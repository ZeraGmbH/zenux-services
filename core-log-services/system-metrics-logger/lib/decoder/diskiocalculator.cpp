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
    valueSince.bytesRead = nextValues.bytesRead - m_currentValues.bytesRead;
    valueSince.bytesWritten = nextValues.bytesWritten - m_currentValues.bytesWritten;
    m_currentValues = nextValues;
    return valueSince;
}
