#ifndef TEST_LOGSTRATEGYFUTURE_H
#define TEST_LOGSTRATEGYFUTURE_H

#include <QObject>

class test_logstrategyfuture : public QObject
{
    Q_OBJECT
private slots:
    void test_someNaNInput();
    void test_allNaNInput();
    void test_allZeroInput();
};

#endif // TEST_LOGSTRATEGYFUTURE_H
