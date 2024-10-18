#ifndef TEST_PROC_PID_DECODER_H
#define TEST_PROC_PID_DECODER_H

#include <QObject>

class test_proc_pid_decoder : public QObject
{
    Q_OBJECT
private slots:
    void checkInvalidProcPath();
    void checkValidProcPath();
};

#endif // TEST_PROC_PID_DECODER_H
