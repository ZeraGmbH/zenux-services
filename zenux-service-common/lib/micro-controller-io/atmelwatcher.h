#ifndef ATMELWATCHER_H
#define ATMELWATCHER_H

#include "atmelwatcherinterface.h"
#include <QTimer>
#include <QString>

class cAtmelWatcher: public AtmelWatcherInterface
{
    Q_OBJECT
public:
    cAtmelWatcher(QString devNode);
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

#endif // ATMELWATCHER_H
