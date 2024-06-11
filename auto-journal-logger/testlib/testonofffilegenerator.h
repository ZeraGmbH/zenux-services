#ifndef TESTONOFFFILEGENERATOR_H
#define TESTONOFFFILEGENERATOR_H

#include <QString>

class TestOnOffFileGenerator
{
public:
    static void createOnFile();
    static void deleteOnFile();

    static QString getTestLogPath();
    static void deleteLogFile();
};

#endif // TESTONOFFFILEGENERATOR_H
