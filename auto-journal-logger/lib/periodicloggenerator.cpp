#include "periodicloggenerator.h"
#include <timerfactoryqt.h>

PeriodicLogGenerator::PeriodicLogGenerator(AbstractLogCreatorPtr logCreator) :
    m_logCreator(std::move(logCreator))
{
}

bool PeriodicLogGenerator::saveLogFileNow()
{
    return m_logCreator->storeLogs();
}

void PeriodicLogGenerator::startPeriodicLog(int storeFreqMs)
{
    saveLogFileNow();
    m_timer = TimerFactoryQt::createPeriodic(storeFreqMs);
    connect(m_timer.get(), &TimerTemplateQt::sigExpired,
            this, &PeriodicLogGenerator::onSaveTimer);
    m_timer->start();
}

void PeriodicLogGenerator::stopPeriodicLog()
{
    m_timer->stop();
}

void PeriodicLogGenerator::onSaveTimer()
{
    saveLogFileNow();
}
