#include "logcreatorjournalsimple.h"

LogCreatorJournalSimple::LogCreatorJournalSimple(QString logFileFullName) :
    m_logFileFullName(logFileFullName)
{
}

bool LogCreatorJournalSimple::storeLogs()
{
    qInfo("Writing journal to %s...", qPrintable(m_logFileFullName));
    QString command = "journalctl -o short-precise --boot >> " + m_logFileFullName;
    if(system(qPrintable(command)) == 0) {
        qInfo("Journal written.");
        return true;
    }
    qWarning("AutoJournalLogger: System command 'journalctl' error");
    return false;
}
