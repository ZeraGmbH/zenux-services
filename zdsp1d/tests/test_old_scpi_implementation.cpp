#include "test_old_scpi_implementation.h"
#include "scpi-zdsp.h"
#include "scpicmdinterpreter.h"
#include "scpicallbackmock.h"
#include <QTest>

QTEST_MAIN(test_old_scpi_implementation)

extern cNode* InitCmdTree();

void test_old_scpi_implementation::query()
{
    ScpiParserZdsp1d parser;
    ScpiCallbackMock scpiMock(&parser);
    ScpiCmdInterpreter cmdInterpreter(&scpiMock, InitCmdTree(), &parser);
    QString scpi("SYSTEM:VERSION:DEVICE?");
    cmdInterpreter.CmdExecute(scpi);

    QCOMPARE(scpiMock.m_queriesReceived.count(), 1);
    QCOMPARE(scpiMock.m_queriesReceived[0], GetDeviceVersion);
}

void test_old_scpi_implementation::cmd()
{
    ScpiParserZdsp1d parser;
    ScpiCallbackMock scpiMock(&parser);
    ScpiCmdInterpreter cmdInterpreter(&scpiMock, InitCmdTree(), &parser);
    QString scpi("SYSTEM:DSP:SAMPLING A,B,C;");
    cmdInterpreter.CmdExecute(scpi);

    QCOMPARE(scpiMock.m_commandsReceived.count(), 1);
    QCOMPARE(scpiMock.m_commandsReceived[0].type, SetSamplingSystem);
    //QCOMPARE(scpiMock.m_commandsReceived[0].cmd, scpi);
}
