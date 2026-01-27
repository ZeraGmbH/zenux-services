#ifndef CTRLHEARTBEATWAIT_H
#define CTRLHEARTBEATWAIT_H

#include "abstractctrlheartbeatwait.h"
#include <timertemplateqt.h>
#include <QString>

class CtrlHeartbeatWait : public AbstractCtrlHeartbeatWait
{
    Q_OBJECT
public:
    CtrlHeartbeatWait(const QString &devNode);
public slots:
    void start() override;

private slots:
    void doTimeout();
    void doAtmelTest();
private:
    void stopTimers();

    QString m_sDeviceNode;
    TimerTemplateQtPtr m_TimerTO;
    TimerTemplateQtPtr m_TimerPeriod;
};

#endif // CTRLHEARTBEATWAIT_H
