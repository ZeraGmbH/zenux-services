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
    void initLogComponents(std::vector<std::unique_ptr<LogComponent> > &logComponents);
    void startLogComponentsTimer(int pollMs);

private slots:
    void onLogComponentsTimer();
private:

    // new style??
    std::vector<std::unique_ptr<LogComponent>> m_logComponents;
    TimerTemplateQtPtr m_logComponentsTimer;
};

#endif // SYSTEMMETRICS_H
