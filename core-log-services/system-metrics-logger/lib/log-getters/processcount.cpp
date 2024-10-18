#include "processcount.h"
#include "procpiddecoder.h"

bool ProcessCount::canGetValue()
{
    return true;
}

float ProcessCount::getValue()
{
    QVector<int> pids = ProcPidDecoder::getPids();
    return pids.count();
}
