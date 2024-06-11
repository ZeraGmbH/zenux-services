#ifndef AUTOJOURNALLOGGER_H
#define AUTOJOURNALLOGGER_H

#include <timertemplateqt.h>
#include <QObject>

class AutoJournalLogger : public QObject
{
    Q_OBJECT
public:
    AutoJournalLogger(QString logFileFullName);
    QString getLogFileFullName();

    bool saveLogFileNow();
    void startPeriodicLog(int storeFreqMs);
    void stopPeriodicLog();

private slots:
    void onSaveTimer();
private:
    QString m_logFileFullName;
    TimerTemplateQtPtr m_timer;
};

#endif // AUTOJOURNALLOGGER_H
