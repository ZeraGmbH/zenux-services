#ifndef DSPINTERRUPTLOGGER_H
#define DSPINTERRUPTLOGGER_H

#include "timertemplateqt.h"
#include <QObject>

class DspInterruptLogger : public QObject
{
    Q_OBJECT
public:
    DspInterruptLogger(int periodMs);
    void addInterruptCount(int perIntInterruptCount);
signals:
    void sigLogNewData(int minInts, int maxInts, float avgInts, int intCount);
private slots:
    void onTimer();
private:
    TimerTemplateQtPtr m_timer;
    QList<int> m_interruptCounts;
};

#endif // DSPINTERRUPTLOGGER_H
