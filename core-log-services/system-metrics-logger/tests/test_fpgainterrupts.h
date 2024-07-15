#ifndef TEST_FPGAINTERRUPTS_H
#define TEST_FPGAINTERRUPTS_H

#include <QObject>

class test_fpgainterrupts : public QObject
{
    Q_OBJECT
private slots:
    void test_interrupts();
    void test_multipleTicks();
    void test_invalidFile();
    void test_noFpgaInInterrupts();
};

#endif // TEST_FPGAINTERRUPTS_H
