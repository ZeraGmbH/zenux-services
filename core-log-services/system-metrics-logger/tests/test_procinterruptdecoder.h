#ifndef TEST_PROCINTERRUPTDECODER_H
#define TEST_PROCINTERRUPTDECODER_H

#include <QObject>

class test_procinterruptdecoder : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void decodeSingleLineInterruptInfo();
    void decodeTestProcInterruptInfo();
};

#endif // TEST_PROCINTERRUPTDECODER_H
