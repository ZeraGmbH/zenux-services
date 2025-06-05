#include "test_tinyserverscpicmdinterface.h"
#include "tinyzscpicmdinterface.h"
#include "proxyclientfornetmessagetest.h"
#include <QTest>

QTEST_MAIN(test_tinyserverscpicmdinterface)

void test_tinyserverscpicmdinterface::noParam()
{
    std::shared_ptr<ProxyClientForNetMessageTest> client = std::make_shared<ProxyClientForNetMessageTest>();
    TinyZScpiCmdInterface interface(client);

    interface.scpiCommand("CMD");
    const QList<ProxyClientForNetMessageTest::ScpiParts> &netCmds = client->getTransmittedMessages();
    QCOMPARE(netCmds[0].cmd, "CMD");
    QCOMPARE(netCmds[0].param, "");
}

void test_tinyserverscpicmdinterface::noParamSemicolon()
{
    std::shared_ptr<ProxyClientForNetMessageTest> client = std::make_shared<ProxyClientForNetMessageTest>();
    TinyZScpiCmdInterface interface(client);

    interface.scpiCommand("CMD;");
    const QList<ProxyClientForNetMessageTest::ScpiParts> &netCmds = client->getTransmittedMessages();
    QCOMPARE(netCmds[0].cmd, "CMD");
    QCOMPARE(netCmds[0].param, "");
}

void test_tinyserverscpicmdinterface::oneParam()
{
    std::shared_ptr<ProxyClientForNetMessageTest> client = std::make_shared<ProxyClientForNetMessageTest>();
    TinyZScpiCmdInterface interface(client);

    interface.scpiCommand("CMD PARAM");
    const QList<ProxyClientForNetMessageTest::ScpiParts> &netCmds = client->getTransmittedMessages();
    QCOMPARE(netCmds[0].cmd, "CMD");
    QCOMPARE(netCmds[0].param, "PARAM");
}

void test_tinyserverscpicmdinterface::oneParamEndsWithSemicolon()
{
    std::shared_ptr<ProxyClientForNetMessageTest> client = std::make_shared<ProxyClientForNetMessageTest>();
    TinyZScpiCmdInterface interface(client);

    interface.scpiCommand("CMD PARAM;");
    const QList<ProxyClientForNetMessageTest::ScpiParts> &netCmds = client->getTransmittedMessages();
    QCOMPARE(netCmds[0].cmd, "CMD");
    QCOMPARE(netCmds[0].param, "PARAM");
}

void test_tinyserverscpicmdinterface::paramsSemicolonSeparated()
{
    std::shared_ptr<ProxyClientForNetMessageTest> client = std::make_shared<ProxyClientForNetMessageTest>();
    TinyZScpiCmdInterface interface(client);

    interface.scpiCommand("CMD PARAM1;PARAM2");
    const QList<ProxyClientForNetMessageTest::ScpiParts> &netCmds = client->getTransmittedMessages();
    QCOMPARE(netCmds[0].cmd, "CMD");
    QCOMPARE(netCmds[0].param, "PARAM1;PARAM2");
}

void test_tinyserverscpicmdinterface::paramsLineSeparated()
{
    std::shared_ptr<ProxyClientForNetMessageTest> client = std::make_shared<ProxyClientForNetMessageTest>();
    TinyZScpiCmdInterface interface(client);

    interface.scpiCommand("CMD PARAM1\nPARAM2");
    const QList<ProxyClientForNetMessageTest::ScpiParts> &netCmds = client->getTransmittedMessages();
    QCOMPARE(netCmds[0].cmd, "CMD");
    QCOMPARE(netCmds[0].param, "PARAM1\nPARAM2");
}
