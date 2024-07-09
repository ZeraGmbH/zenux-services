#include "cpuload.h"
#include "procstatdecoder.h"
#include <cmath>

CpuLoad::CpuLoad(int coreIdx) : m_coreIdx(coreIdx)
{
}

void CpuLoad::calcNextValues()
{
    const QMap<int, CpuTimingValues> allTimings = ProcStatDecoder::getCpuTimings();
    for(auto iter=allTimings.cbegin(); iter!=allTimings.cend(); iter++) {
        const int timingNum = iter.key();
        const CpuTimingValues& timings = iter.value();
        // inspired by https://gitlab.xfce.org/panel-plugins/xfce4-cpugraph-plugin/-/blob/master/panel-plugin/os.cc
        // Simplified: just Linux & no CPU hotplug
        LoadRelevantData currLoadRelData;
        currLoadRelData.m_loadRelevantTime =
            timings.m_systemTime +
            timings.m_irqTime +
            timings.m_softIrqTime +
            timings.m_userTime +
            timings.m_niceTime;
        currLoadRelData.m_totalTime =
            currLoadRelData.m_loadRelevantTime +
            timings.m_idleTime + timings.m_iowaitTime;

        float totalDiff = (float)(currLoadRelData.m_totalTime - m_prevLoadRelData[timingNum].m_totalTime);
        float loadRelevantDiff = (float)(currLoadRelData.m_loadRelevantTime - m_prevLoadRelData[timingNum].m_loadRelevantTime);
        float load = totalDiff == 0.0 ? 0.0 : loadRelevantDiff / totalDiff;
        m_loadMap[timingNum] = load;
        m_prevLoadRelData[timingNum] = currLoadRelData;
    }
}

float CpuLoad::getValue()
{
    calcNextValues();
    return m_loadMap[m_coreIdx] * 100;
}

bool CpuLoad::canGetValue()
{
    calcNextValues();
    if (m_loadMap.contains(m_coreIdx))
        return true;
    else
        return false;
}
