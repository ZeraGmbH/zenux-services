#include "autojournallogger.h"
#include <timerfactoryqt.h>

AutoJournalLogger::AutoJournalLogger(QString logFileFullName) :
    m_logFileFullName(logFileFullName)
{
}

void AutoJournalLogger::startPeriodicLog(int storeFreqMs)
{
    saveLogFileNow();
    m_timer = TimerFactoryQt::createPeriodic(storeFreqMs);
    connect(m_timer.get(), &TimerTemplateQt::sigExpired,
            this, &AutoJournalLogger::onSaveTimer);
    m_timer->start();
}

void AutoJournalLogger::stopPeriodicLog()
{
    m_timer->stop();
}

QString AutoJournalLogger::getLogFileFullName()
{
    return m_logFileFullName;
}

void AutoJournalLogger::onSaveTimer()
{
    saveLogFileNow();
}

bool AutoJournalLogger::saveLogFileNow()
{
    QString command = "journalctl -o short-precise --boot >> " + getLogFileFullName();
    if(system(qPrintable(command)) == 0)
        return true;
    qWarning("AutoJournalLogger: System command 'journalctl' error");
    return false;
}
