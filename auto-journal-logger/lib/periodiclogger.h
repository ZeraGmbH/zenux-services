#ifndef PERIODICLOGGER_H
#define PERIODICLOGGER_H

#include "abstractlogcreator.h"
#include <timertemplateqt.h>
#include <QObject>

class PeriodicLogger : public QObject
{
    Q_OBJECT
public:
    PeriodicLogger(AbstractLogCreatorPtr logCreator);
    bool saveLogFileNow();
    void startPeriodicLog(int storeFreqMs);
    void stopPeriodicLog();

private slots:
    void onSaveTimer();
private:
    AbstractLogCreatorPtr m_logCreator;
    TimerTemplateQtPtr m_timer;
};

#endif // PERIODICLOGGER_H
