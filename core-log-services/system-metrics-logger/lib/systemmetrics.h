#ifndef SYSTEMMETRICS_H
#define SYSTEMMETRICS_H

#include "totalmemorytracker.h"
#include "logcomponent.h"
#include <timertemplateqt.h>
#include <QObject>
#include <vector>

class SystemMetrics : public QObject
{
    Q_OBJECT
public:
    TotalMemoryTracker *getTotalMemoryTracker();
    void startMemoryPollTimer(int logIntervalMs);
    void startLogComponentsTimer(int pollMs);
    void initLogComponents();
signals:
    void sigNewCpuValues();
    void sigNewMemValues();

private slots:
    void onMemoryTimer();
    void onLogComponentsTimer();
private:
    TimerTemplateQtPtr m_memoryPollTimer;
    TotalMemoryTracker m_totalMemoryTracker;

    // new style??
    std::vector<std::unique_ptr<LogComponent>> m_logComponents;
    TimerTemplateQtPtr m_logComponentsTimer;
};

#endif // SYSTEMMETRICS_H
