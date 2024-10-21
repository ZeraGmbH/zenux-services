#ifndef SYSTEMMETRICS_H
#define SYSTEMMETRICS_H

#include "logcomponent.h"
#include "diskreadwritevalues.h"
#include <timertemplateqt.h>
#include <QObject>
#include <vector>

class SystemMetrics : public QObject
{
    Q_OBJECT
public:
    void startLogComponentsTimer();
    void initLogComponents();

private slots:
    void onLogComponentsTimer();
private:
    void init1sClocked10sResultLoggers();
    void init1sClocked60sResultLoggers();
    void init60sClockedOneShotResultLoggers();
    std::vector<std::unique_ptr<LogComponent<float>>> m_logComponents;
    std::vector<std::unique_ptr<LogComponent<DiskValuesProcesses>>> m_logComponents60s;
    TimerTemplateQtPtr m_1sPeriodicTimer;
    int m_60sCounter = 0;
};

#endif // SYSTEMMETRICS_H
