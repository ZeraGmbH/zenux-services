#ifndef TEST_DSP_VAR_OFFSET_CALC_H
#define TEST_DSP_VAR_OFFSET_CALC_H

#include <QObject>

class test_dsp_var_offset_calc : public QObject
{
    Q_OBJECT
private slots:
    void unequalVarNames();
    void equalVarNames();
    void checkCaseSensivity();
    void equalVarNamesIgnoreSpaces();
    void offsetPlus1();
    void offsetPlus0();
    void rejectNegativeOffsets();

    void justOffset();

    void offsetHexNo0x();
    void offsetHex();
    void offsetInvalid();
};

#endif // TEST_DSP_VAR_OFFSET_CALC_H
