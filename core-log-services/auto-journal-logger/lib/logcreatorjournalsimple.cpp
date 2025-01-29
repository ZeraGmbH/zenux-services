#include "logcreatorjournalsimple.h"
#include <QDateTime>

bool LogCreatorJournalSimple::storeLogs(QString destinationDir)
{
    QString fileName = destinationDir + "/journal.log";
    for(int boot = 0; boot<3; boot++) {
        QString command = QString("journalctl -o short-precise --boot -%1 >> '%2-%1'").arg(boot).arg(fileName);
        if(system(qPrintable(command)) != 0 && boot == 0) // at least one boot must be there
            return false;
    }
    return true;
}
