#ifndef TESTLOGCREATOR_H
#define TESTLOGCREATOR_H

#include <abstractlogcreator.h>
#include <QString>

class TestLogCreator : public AbstractLogCreator
{
public:
    TestLogCreator(QString logFileFullName);
    bool storeLogs() override;
private:
    QString m_logFileFullName;
};

#endif // TESTLOGCREATOR_H
