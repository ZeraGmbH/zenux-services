#include "testlogcreator.h"
#include <QFile>

TestLogCreator::TestLogCreator(QString logFileFullName) :
    m_logFileFullName(logFileFullName)
{
}

bool TestLogCreator::storeLogs()
{
    QFile file(m_logFileFullName);
    if(file.open(QFile::WriteOnly)) {
        file.write("Testlog content");
        return true;
    }
    return false;
}
