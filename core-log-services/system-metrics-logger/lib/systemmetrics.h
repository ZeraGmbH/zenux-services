#ifndef SYSTEMMETRICS_H
#define SYSTEMMETRICS_H

#include "cpuload.h"
#include "totalmemorytracker.h"
#include "logcomponent.h"
#include <timertemplateqt.h>
#include <QObject>

class SystemMetrics : public QObject
{
    Q_OBJECT
public:
    CpuLoad *getCpuLoad();
    TotalMemoryTracker *getTotalMemoryTracker();
    void startCpuLoadPollTimer(int pollMs);
    void startMemoryPollTimer(int logIntervalMs);
    void startLogComponentsTimer(int pollMs);
    void initLogComponents();
signals:
    void sigNewCpuValues();
    void sigNewMemValues();

private slots:
    void onCpuLoadTimer();
    void onMemoryTimer();
    void onLogComponentsTimer();
private:
    TimerTemplateQtPtr m_cpuLoadPollTimer;
    TimerTemplateQtPtr m_memoryPollTimer;
    CpuLoad m_cpuLoad;
    TotalMemoryTracker m_totalMemoryTracker;
    std::unique_ptr<LogComponent> m_cpuTempComponent;
    TimerTemplateQtPtr m_logComponentsTimer;
};

#endif // SYSTEMMETRICS_H
