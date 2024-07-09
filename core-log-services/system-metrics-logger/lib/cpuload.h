#ifndef CPULOAD_H
#define CPULOAD_H

#include "abstractlogvaluegetter.h"
#include <QMap>

class CpuLoad : public AbstractLogValueGetter
{
public:
    CpuLoad(int coreIdx);
    void calcNextValues();
    float getValue();
    bool canGetValue();

private:
    struct LoadRelevantData
    {
        quint64 m_totalTime = 0;
        quint64 m_loadRelevantTime = 0;
    };
    QMap<int, float> m_loadMap;
    QMap<int, LoadRelevantData> m_prevLoadRelData;

    int m_coreIdx = 0;
};

#endif // CPULOAD_H
