#ifndef AUTOJOURNALLOGGER_H
#define AUTOJOURNALLOGGER_H

#include <timertemplateqt.h>
#include <QObject>

class AutoJournalLogger : public QObject
{
    Q_OBJECT
public:
    AutoJournalLogger(QString logFileDir);
    QString getLogFilePath();
    bool saveLogFileNow();
    void startPeriodicLog(int storeFreqMs);
    void stopPeriodicLog();
private slots:
    void onSaveTimer();
private:
    QString m_logFileDir;
    TimerTemplateQtPtr m_timer;
};

#endif // AUTOJOURNALLOGGER_H
