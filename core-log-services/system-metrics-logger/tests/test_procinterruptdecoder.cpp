#include "test_procinterruptdecoder.h"
#include "procinterruptdecoder.h"
#include "testsysteminfofilelocator.h"
#include <QTest>

QTEST_MAIN(test_procinterruptdecoder)

void test_procinterruptdecoder::initTestCase()
{
    TestSystemInfoFileLocator::setProcInterruptFile(":/procInterruptsMT310s2");
}

void test_procinterruptdecoder::decodeSingleLineInterruptInfo()
{
    QString interruptLine = "120:      91988          0          0          0  gpio-mxc  14 Level     zFPGA1dsp1, zFPGA1ec, zFPGA1reg, zFPGA1msg";
    quint32 interruptValue = ProcInterruptDecoder::decodeSingleInterruptValue(interruptLine);
    QCOMPARE(interruptValue, 91988);
}

void test_procinterruptdecoder::decodeTestProcInterruptInfo()
{
    InterruptValues testInterruptValues = ProcInterruptDecoder::getCurrentInterrupts();
    QCOMPARE(testInterruptValues.fpgaInterrupts, 91988);
}
