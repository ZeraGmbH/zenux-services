#ifndef TEST_PROC_PID_IO_DECODER_H
#define TEST_PROC_PID_IO_DECODER_H

#include <QObject>

class test_proc_pid_io_decoder :  public QObject
{
    Q_OBJECT
private slots:
    void pidInvalid();
    void pidValid1();
    void pidValid1TrailingPathSeparator();
    void pidValid2();
    void pidValid12_Invalid3();
};

#endif // TEST_PROC_PID_IO_DECODER_H
