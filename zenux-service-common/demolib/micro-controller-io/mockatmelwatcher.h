#ifndef MOCKATMELWATCHER_H
#define MOCKATMELWATCHER_H

#include <atmelwatcherinterface.h>

class MockAtmelWatcher : public AtmelWatcherInterface
{
    Q_OBJECT
public slots:
    void start() override;
};

#endif // MOCKATMELWATCHER_H
