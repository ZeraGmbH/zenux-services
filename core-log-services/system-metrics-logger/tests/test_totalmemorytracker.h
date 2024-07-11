#ifndef TEST_TOTALMEMORYTRACKER_H
#define TEST_TOTALMEMORYTRACKER_H

#include <QObject>

class test_totalmemorytracker : public QObject
{
    Q_OBJECT
private slots:
    void test_invalid_values();
    void test_memory_used();
};

#endif // TEST_TOTALMEMORYTRACKER_H
