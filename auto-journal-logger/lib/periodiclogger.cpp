#include "periodiclogger.h"
#include <timerfactoryqt.h>

PeriodicLogger::PeriodicLogger(AbstractLogCreatorPtr logCreator) :
    m_logCreator(std::move(logCreator))
{
}

bool PeriodicLogger::saveLogFileNow()
{
    return m_logCreator->storeLogs();
}

void PeriodicLogger::startPeriodicLog(int storeFreqMs)
{
    saveLogFileNow();
    m_timer = TimerFactoryQt::createPeriodic(storeFreqMs);
    connect(m_timer.get(), &TimerTemplateQt::sigExpired,
            this, &PeriodicLogger::onSaveTimer);
    m_timer->start();
}

void PeriodicLogger::stopPeriodicLog()
{
    m_timer->stop();
}

void PeriodicLogger::onSaveTimer()
{
    saveLogFileNow();
}
