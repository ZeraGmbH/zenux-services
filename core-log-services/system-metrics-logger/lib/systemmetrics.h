#ifndef SYSTEMMETRICS_H
#define SYSTEMMETRICS_H

#include "logcomponent.h"
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
    std::vector<std::unique_ptr<LogComponent<float>>> m_logComponents;
    TimerTemplateQtPtr m_1sPeriodicTimer;
};

#endif // SYSTEMMETRICS_H
