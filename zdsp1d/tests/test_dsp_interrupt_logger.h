#ifndef TEST_DSP_INTERRUPT_LOGGER_H
#define TEST_DSP_INTERRUPT_LOGGER_H

#include <QObject>

class test_dsp_interrupt_logger : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void noSignalsForNoValuesAdded();
    void oneValueAdded();
    void twoValuesAdded();
    void twoValuesAddedTwoTimeSlots();
};

#endif // TEST_DSP_INTERRUPT_LOGGER_H
