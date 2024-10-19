#ifndef TEST_PROC_PID_DETAILS_H
#define TEST_PROC_PID_DETAILS_H

#include <QObject>

class test_proc_pid_details : public QObject
{
    Q_OBJECT
private slots:
    void checkForRealSystemd();
    void checkDifferentFormats();
};

#endif // TEST_PROC_PID_DETAILS_H
