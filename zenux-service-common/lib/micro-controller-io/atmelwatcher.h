#ifndef ATMELWATCHER_H
#define ATMELWATCHER_H

#include "atmelwatcherinterface.h"
#include <QTimer>
#include <QString>

class cAtmelWatcher: public AtmelWatcherInterface
{
    Q_OBJECT
public:
    cAtmelWatcher(quint8 dlevel, QString devNode, int timeout, int tperiod);
public slots:
    void start() override;
private:
    QString m_sDeviceNode;
    QTimer m_TimerTO;
    QTimer m_TimerPeriod;
    quint8 m_nDebugLevel;
    int fd;
private slots:
    void doTimeout();
    void doAtmelTest();
};

#endif // ATMELWATCHER_H
