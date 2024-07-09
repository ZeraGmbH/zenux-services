#include "cpuload.h"
#include "procstatdecoder.h"
#include <cmath>

CpuLoad::CpuLoad(int coreIdx) : m_coreIdx(coreIdx)
{
}

float CpuLoad::getValue()
{
    const CpuTimingValues singleTiming = ProcStatDecoder::getCpuTimingsSingle(m_coreIdx);
    LoadRelevantData currLoadRelData;
    currLoadRelData.m_loadRelevantTime =
        singleTiming.m_systemTime +
        singleTiming.m_irqTime +
        singleTiming.m_softIrqTime +
        singleTiming.m_userTime +
        singleTiming.m_niceTime;
    currLoadRelData.m_totalTime =
        currLoadRelData.m_loadRelevantTime +
        singleTiming.m_idleTime + singleTiming.m_iowaitTime;

    float totalDiff = (float)(currLoadRelData.m_totalTime - m_prevLoadRelData.m_totalTime);
    float loadRelevantDiff = (float)(currLoadRelData.m_loadRelevantTime - m_prevLoadRelData.m_loadRelevantTime);
    float load = totalDiff == 0.0 ? 0.0 : loadRelevantDiff / totalDiff;

    m_prevLoadRelData = currLoadRelData;
    return load * 100;
}

bool CpuLoad::canGetValue()
{
    return ProcStatDecoder::procStatOk(m_coreIdx);
}
