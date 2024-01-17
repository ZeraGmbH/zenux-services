#ifndef MOCKATMELWATCHER_H
#define MOCKATMELWATCHER_H

#include <abstractctrlheartbeatwait.h>

class MockCtrlHeartbeatWait : public AbstractCtrlHeartbeatWait
{
    Q_OBJECT
public slots:
    void start() override;
};

#endif // MOCKATMELWATCHER_H
