#ifndef LOGCREATORJOURNALSIMPLE_H
#define LOGCREATORJOURNALSIMPLE_H

#include "abstractlogcreator.h"
#include <QString>

class LogCreatorJournalSimple : public AbstractLogCreator
{
public:
    virtual ~LogCreatorJournalSimple() = default;
    bool storeLogs(QString destinationDir) override;
};

#endif // LOGCREATORJOURNALSIMPLE_H
