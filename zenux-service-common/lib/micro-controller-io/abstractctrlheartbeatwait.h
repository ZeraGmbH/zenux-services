#ifndef ABSTRACTCTRLHEARTBEATWAIT_H
#define ABSTRACTCTRLHEARTBEATWAIT_H

#include <QObject>
#include <memory>

class AbstractCtrlHeartbeatWait : public QObject
{
    Q_OBJECT
public slots:
    virtual void start() = 0;
signals:
    void sigTimeout();
    void sigRunning();
};

typedef std::unique_ptr<AbstractCtrlHeartbeatWait> AbstractCtrlHeartbeatWaitPtr;

#endif // ABSTRACTCTRLHEARTBEATWAIT_H
