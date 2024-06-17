#include "logcreatorjournalsimple.h"
#include <QDateTime>

bool LogCreatorJournalSimple::storeLogs(QString dir)
{
    QDateTime now = QDateTime::currentDateTime();
    QString fileName = dir + "/zenux-" + now.toString("yyyy_MM_dd-HH_mm_ss") + ".log";
    QString command = "journalctl -o short-precise --boot >> " + fileName;
    if(system(qPrintable(command)) == 0)
        return true;
    return false;
}
