#ifndef TESTLOGCREATOR_H
#define TESTLOGCREATOR_H

#include <abstractlogcreator.h>
#include <QString>

class TestLogCreator : public AbstractLogCreator
{
public:
    TestLogCreator(QString testFileName);
    virtual ~TestLogCreator() = default;
    bool storeLogs(QString destinationDir) override;
private:
    QString m_testFileName;
};

#endif // TESTLOGCREATOR_H
