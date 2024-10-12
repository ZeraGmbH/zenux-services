#include "diskreadtotal.h"

DiskReadTotal::DiskReadTotal(std::unique_ptr<DiskIoTotalCalculator> diskTotalCalc) :
    m_diskTotalCalc(std::move(diskTotalCalc))
{
}

bool DiskReadTotal::canGetValue()
{
    return true;
}

float DiskReadTotal::getValue()
{
    quint64 kiBytes = m_diskTotalCalc->getReadWriteSinceLast().bytesRead / 1024; // KiB (KB)
    return float(kiBytes);
}
