#include "journallogcreator.h"

JournalLogCreator::JournalLogCreator(QString logFileFullName) :
    m_logFileFullName(logFileFullName)
{
}

bool JournalLogCreator::storeLogs()
{
    QString command = "journalctl -o short-precise --boot >> " + m_logFileFullName;
    if(system(qPrintable(command)) == 0)
        return true;
    qWarning("AutoJournalLogger: System command 'journalctl' error");
    return false;
}
