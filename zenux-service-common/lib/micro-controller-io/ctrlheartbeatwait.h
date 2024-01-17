#ifndef CTRLHEARTBEATWAIT_H
#define CTRLHEARTBEATWAIT_H

#include "abstractctrlheartbeatwait.h"
#include <QTimer>
#include <QString>

class CtrlHeartbeatWait : public AbstractCtrlHeartbeatWait
{
    Q_OBJECT
public:
    CtrlHeartbeatWait(QString devNode);
public slots:
    void start() override;
private:
    QString m_sDeviceNode;
    QTimer m_TimerTO;
    QTimer m_TimerPeriod;
    int fd;
private slots:
    void doTimeout();
    void doAtmelTest();
};

#endif // CTRLHEARTBEATWAIT_H
