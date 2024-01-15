#ifndef ATMELWATCHERMOCK_H
#define ATMELWATCHERMOCK_H

#include <atmelwatcherinterface.h>

class AtmelWatcherMock : public AtmelWatcherInterface
{
    Q_OBJECT
public slots:
    void start() override;
};

#endif // ATMELWATCHERMOCK_H
