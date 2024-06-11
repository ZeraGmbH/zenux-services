#ifndef LOGCREATORJOURNALSIMPLE_H
#define LOGCREATORJOURNALSIMPLE_H

#include "abstractlogcreator.h"
#include <QString>

class LogCreatorJournalSimple : public AbstractLogCreator
{
public:
    LogCreatorJournalSimple(QString logFileFullName);
    bool storeLogs() override;
private:
    QString m_logFileFullName;
};

#endif // LOGCREATORJOURNALSIMPLE_H
