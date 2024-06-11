#ifndef JOURNALLOGCREATOR_H
#define JOURNALLOGCREATOR_H

#include "abstractlogcreator.h"
#include <QString>

class JournalLogCreator : public AbstractLogCreator
{
public:
    JournalLogCreator(QString logFileFullName);
    bool storeLogs() override;
private:
    QString m_logFileFullName;
};

#endif // JOURNALLOGCREATOR_H
