#ifndef ATMELWATCHERINTERFACE_H
#define ATMELWATCHERINTERFACE_H

#include <QObject>

class AtmelWatcherInterface : public QObject
{
    Q_OBJECT
public slots:
    virtual void start() = 0;
signals:
    void timeout();
    void running();
};

#endif // ATMELWATCHERINTERFACE_H
