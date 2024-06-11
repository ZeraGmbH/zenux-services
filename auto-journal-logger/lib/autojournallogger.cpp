#include "autojournallogger.h"
#include <timerfactoryqt.h>

AutoJournalLogger::AutoJournalLogger(AbstractLogCreatorPtr logCreator) :
    m_logCreator(std::move(logCreator))
{
}

bool AutoJournalLogger::saveLogFileNow()
{
    return m_logCreator->storeLogs();
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

void AutoJournalLogger::onSaveTimer()
{
    saveLogFileNow();
}
