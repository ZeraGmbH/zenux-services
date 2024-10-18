#ifndef test_log_collector_floating_numbers_H
#define test_log_collector_floating_numbers_H

#include <QObject>

class test_log_collector_floating_numbers : public QObject
{
    Q_OBJECT
private slots:
    void noReturnOnOneBelow();
    void returnFirstFull();
    void noReturnOnAfterFirstFull();
    void returnSecondFull();
    void ignoreNan();
    void ignoreInf();
    void returnFirstFullDouble();
};

#endif // test_log_collector_floating_numbers_H
