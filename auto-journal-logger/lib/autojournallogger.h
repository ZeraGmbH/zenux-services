#ifndef AUTOJOURNALLOGGER_H
#define AUTOJOURNALLOGGER_H

#include "abstractlogcreator.h"
#include <timertemplateqt.h>
#include <QObject>

class AutoJournalLogger : public QObject
{
    Q_OBJECT
public:
    AutoJournalLogger(AbstractLogCreatorPtr logCreator);
    bool saveLogFileNow();
    void startPeriodicLog(int storeFreqMs);
    void stopPeriodicLog();

private slots:
    void onSaveTimer();
private:
    AbstractLogCreatorPtr m_logCreator;
    TimerTemplateQtPtr m_timer;
};

#endif // AUTOJOURNALLOGGER_H
