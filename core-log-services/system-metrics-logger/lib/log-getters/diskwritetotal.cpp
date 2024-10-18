#include "diskwritetotal.h"

DiskWriteTotal::DiskWriteTotal(std::unique_ptr<DiskIoTotalCalculator> diskTotalCalc) :
    m_diskTotalCalc(std::move(diskTotalCalc))
{
}

bool DiskWriteTotal::canGetValue()
{
    return true;
}

float DiskWriteTotal::getValue()
{
    quint64 kiBytes = m_diskTotalCalc->getReadWriteSinceLast().bytesWritten / 1024; // KiB (KB)
    return float(kiBytes);
}
