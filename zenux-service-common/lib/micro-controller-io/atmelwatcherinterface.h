#ifndef ATMELWATCHERINTERFACE_H
#define ATMELWATCHERINTERFACE_H

#include <QObject>
#include <memory>

class AtmelWatcherInterface : public QObject
{
    Q_OBJECT
public slots:
    virtual void start() = 0;
signals:
    void sigTimeout();
    void sigRunning();
};

typedef std::unique_ptr<AtmelWatcherInterface> AtmelWatcherInterfacePrt;

#endif // ATMELWATCHERINTERFACE_H
