#include "testlogcreator.h"
#include <QFile>

TestLogCreator::TestLogCreator(QString testFileName) :
    m_testFileName(testFileName)
{
}

bool TestLogCreator::storeLogs(QString dir)
{
    QFile file(dir + "/" + m_testFileName);
    if(file.open(QFile::WriteOnly)) {
        file.write("Testlog content");
        return true;
    }
    return false;
}
