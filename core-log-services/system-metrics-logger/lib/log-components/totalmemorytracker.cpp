#include "totalmemorytracker.h"
#include "procmeminfodecoder.h"
#include <math.h>

bool TotalMemoryTracker::canGetValue()
{
    MemoryValues memoryValues = ProcMeminfoDecoder::getCurrentMemoryValues();
    if(memoryValues.freeMemory != 0)
        return true;
    return false;
}

float TotalMemoryTracker::getValue()
{
    // calculation taken from https://stackoverflow.com/questions/41224738/how-to-calculate-system-memory-usage-from-proc-meminfo-like-htop
    MemoryValues memoryValues = ProcMeminfoDecoder::getCurrentMemoryValues();
    quint32 usedMemory = memoryValues.totalMemory - (memoryValues.freeMemory + memoryValues.buffers + (memoryValues.cached + memoryValues.sReclaimable - memoryValues.shmem));
    float usedMemoryPercent = calcPercentageOneDecimal(float(usedMemory) / float(memoryValues.totalMemory));
    return usedMemoryPercent;
}

float TotalMemoryTracker::calcPercentageOneDecimal(float value)
{
    float percentageVal = value * 100;
    return round(percentageVal * 10) / 10;
}

