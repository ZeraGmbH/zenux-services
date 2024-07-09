#ifndef CPULOAD_H
#define CPULOAD_H

#include "abstractlogvaluegetter.h"
#include <QMap>

class CpuLoad : public AbstractLogValueGetter
{
public:
    CpuLoad(int coreIdx);
    float getValue();
    bool canGetValue();

private:
    struct LoadRelevantData
    {
        quint64 m_totalTime = 0;
        quint64 m_loadRelevantTime = 0;
    };

    LoadRelevantData m_prevLoadRelData;

    int m_coreIdx = 0;
};

#endif // CPULOAD_H
