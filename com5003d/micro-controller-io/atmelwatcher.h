#ifndef ATMELWATCHER_H
#define ATMELWATCHER_H

#include <QObject>
#include <QTimer>

class QString;

class cAtmelWatcher: public QObject
{
    Q_OBJECT
public:
    cAtmelWatcher(quint8 dlevel, QString devNode, int timeout, int tperiod);
    virtual ~cAtmelWatcher();

public slots:
    void start();

signals:
    void timeout();
    void running();

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
