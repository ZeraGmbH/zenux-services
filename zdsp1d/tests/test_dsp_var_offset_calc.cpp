#include "test_dsp_var_offset_calc.h"
#include "dspvaroffsetcalc.h"
#include <QTest>

QTEST_MAIN(test_dsp_var_offset_calc)

void test_dsp_var_offset_calc::unequalVarNames()
{
    int offset = 42;
    QCOMPARE(DspVarOffsetCalc::calcVarOffset("foo", "bar", offset), false);
    QCOMPARE(offset, 42);
}

void test_dsp_var_offset_calc::equalVarNames()
{
    int offset = 42;
    QCOMPARE(DspVarOffsetCalc::calcVarOffset("foo", "foo", offset), true);
    QCOMPARE(offset, 0);
}

void test_dsp_var_offset_calc::checkCaseSensivity()
{
    int offset = 42;
    QCOMPARE(DspVarOffsetCalc::calcVarOffset("foo", "FOO", offset), false);
    QCOMPARE(offset, 42);
}

void test_dsp_var_offset_calc::equalVarNamesIgnoreSpaces()
{
    int offset = 42;
    QCOMPARE(DspVarOffsetCalc::calcVarOffset(" foo ", "foo", offset), true);
    QCOMPARE(offset, 0);

    offset = 42;
    QCOMPARE(DspVarOffsetCalc::calcVarOffset(" foo ", " foo ", offset), true);
    QCOMPARE(offset, 0);
}

void test_dsp_var_offset_calc::offsetPlus1()
{
    int offset = 42;
    QCOMPARE(DspVarOffsetCalc::calcVarOffset("FOO", "FOO + 1", offset), true);
    QCOMPARE(offset, 1);
}

void test_dsp_var_offset_calc::offsetMinus1()
{
    int offset = 42;
    QCOMPARE(DspVarOffsetCalc::calcVarOffset("FOO", "FOO -1", offset), true);
    QCOMPARE(offset, -1);
}

void test_dsp_var_offset_calc::justOffset()
{
    int offset = 42;
    QCOMPARE(DspVarOffsetCalc::calcVarOffset(" ", "+1", offset), true);
    QCOMPARE(offset, +1);
}

void test_dsp_var_offset_calc::offsetHexNo0x()
{
    int offset = 42;
    QCOMPARE(DspVarOffsetCalc::calcVarOffset("FOO", "FOO+ABC", offset), true);
    QCOMPARE(offset, 0xABC);
}

void test_dsp_var_offset_calc::offsetHex()
{
    int offset = 42;
    QCOMPARE(DspVarOffsetCalc::calcVarOffset("FOO", "FOO+0x11", offset), true);
    QCOMPARE(offset, 0x11);
}

void test_dsp_var_offset_calc::offsetInvalid()
{
    int offset = 42;
    QCOMPARE(DspVarOffsetCalc::calcVarOffset("FOO", "FOO+H12", offset), false);
    QCOMPARE(offset, 42);
}
