#include "periodicloggerwrapper.h"
#include "logcreatorjournalsimple.h"

PeriodicLoggerWrapper::PeriodicLoggerWrapper(QObject *parent)
    : QObject(parent)
{
}

void PeriodicLoggerWrapper::startPeriodicLog(QString logFilenameFull, int periodSeconds)
{
    m_logGenerator = std::make_unique<PeriodicLogger>(std::make_unique<LogCreatorJournalSimple>(logFilenameFull));
    m_logGenerator->startPeriodicLog(periodSeconds * 1000);
}

void PeriodicLoggerWrapper::stopPeriodicLog()
{
    m_logGenerator = nullptr;
}

bool PeriodicLoggerWrapper::isLogActive()
{
    return m_logGenerator != nullptr;
}
