#include "logcreatorjournalsimple.h"
#include <QDateTime>

bool LogCreatorJournalSimple::storeLogs(QString dir)
{
    QDateTime now = QDateTime::currentDateTime();
    QString fileName = dir + "/" + now.toString("yyyy_MM_dd-HH_mm_ss");
    for(int boot = 0; boot<3; boot++) {
        QString command = QString("journalctl -o short-precise --boot -%1 >> %2.log-%1").arg(boot).arg(fileName);
        if(system(qPrintable(command)) != 0)
            return false;
    }
    return true;
}
