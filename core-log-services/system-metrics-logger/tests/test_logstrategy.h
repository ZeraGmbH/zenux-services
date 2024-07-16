#ifndef TEST_LOGSTRATEGY_H
#define TEST_LOGSTRATEGY_H

#include <QObject>

class test_logstrategy : public QObject
{
    Q_OBJECT
private slots:
    void test_someNaNInput();
    void test_allNaNInput();
    void test_allZeroInput();
};

#endif // TEST_LOGSTRATEGY_H
