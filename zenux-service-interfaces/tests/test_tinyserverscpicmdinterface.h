#ifndef TEST_TINYSERVERSCPICMDINTERFACE_H
#define TEST_TINYSERVERSCPICMDINTERFACE_H

#include <QObject>

class test_tinyserverscpicmdinterface : public QObject
{
    Q_OBJECT
private slots:
    void noParam();
    void noParamSemicolon();
    void oneParam();
    void oneParamEndsWithSemicolon();
    void paramsSemicolonSeparated();
    void paramsLineSeparated();
};

#endif // TEST_TINYSERVERSCPICMDINTERFACE_H
