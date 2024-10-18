#ifndef TEST_CPUTEMP_H
#define TEST_CPUTEMP_H

#include <QObject>

class test_cputemp : public QObject
{
    Q_OBJECT
private slots:
    void test_temp();
    void test_invalidDirectory();
};

#endif // TEST_CPUTEMP_H
