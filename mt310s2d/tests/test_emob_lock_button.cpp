#include "test_emob_lock_button.h"
#include "hotpluggablecontrollercontainer.h"
#include "mt310s2senseinterface.h"
#include "mt310s2systeminfo.h"
#include "testfactoryi2cctrl.h"
#include "testpcbserver.h"
#include "mocktcpnetworkfactory.h"
#include "zscpi_response_definitions.h"
#include <timemachinefortest.h>
#include <QTest>

QTEST_MAIN(test_emob_lock_button);

void test_emob_lock_button::init()
{
    VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create();
    std::unique_ptr<TestPcbServer> testPcbServer = std::make_unique<TestPcbServer>("mt310s2", tcpNetworkFactory);
    AbstractFactoryI2cCtrlPtr ctrlFactory = std::make_shared<TestFactoryI2cCtrl>(true);
    std::unique_ptr<Mt310s2SystemInfo> systemInfo = std::make_unique<Mt310s2SystemInfo>(ctrlFactory);
    m_scpiInterface = testPcbServer->getSCPIInterface();

    std::unique_ptr<cSenseSettings> senseSettings = std::make_unique<cSenseSettings>(testPcbServer->getConfigReader(), 8);;
    std::unique_ptr<SenseInterfaceCommon> senseInterface = std::make_unique<Mt310s2SenseInterface>(m_scpiInterface,
                                                                                                   testPcbServer->getSettings()->getI2cSettings(),
                                                                                                   senseSettings.get(),
                                                                                                   systemInfo.get(),
                                                                                                   SettingsContainer::createChannelRangeFactory("mt310s2"),
                                                                                                   ctrlFactory);

    HotPluggableControllerContainerPtr emobControllerContainer = std::make_unique<HotPluggableControllerContainer>(testPcbServer->getSettings()->getI2cSettings(),
                                                                                                                   ctrlFactory);
    emobControllerContainer->startActualizeEmobControllers((1<<0), senseSettings.get(), 1000);

    m_systemInterface = std::make_unique<Mt310s2SystemInterface>(testPcbServer.get(),
                                                                 systemInfo.get(),
                                                                 senseSettings.get(),
                                                                 senseInterface.get(),
                                                                 ctrlFactory,
                                                                 std::move(emobControllerContainer));

    m_systemInterface->initSCPIConnection("");
}

void test_emob_lock_button::findSCPIObjects()
{
    QString pressButtonCmd = "SYSTEM:EMOB:PBPRESS;";
    QString lockStateCmd = "SYSTEM:EMOB:LOCKST?";
    QVERIFY(m_scpiInterface->getSCPIObject(pressButtonCmd));
    QVERIFY(m_scpiInterface->getSCPIObject(lockStateCmd));
}

void test_emob_lock_button::pushButton()
{
    QString pressButtonCmd = "SYSTEM:EMOB:PBPRESS;";
    ProtonetCommandPtr protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, false, QByteArray(), 0, pressButtonCmd);
    ScpiObjectPtr scpiObject = m_scpiInterface->getSCPIObject(pressButtonCmd);
    ScpiDelegate* scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE(protoCmd->m_sOutput, ZSCPI::scpiAnswer[ZSCPI::ack]);
}

void test_emob_lock_button::readLockState()
{
    QString cmd = "SYSTEM:EMOB:LOCKST?";
    ProtonetCommandPtr protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, false, QByteArray(), 0, cmd);
    ScpiObjectPtr scpiObject = m_scpiInterface->getSCPIObject(cmd);
    ScpiDelegate* scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE(protoCmd->m_sOutput, "0");

}

void test_emob_lock_button::pressButtonUl1Channel()
{
    QString pressButtonCmd = "SYSTEM:EMOB:PBPRESS UL1;";
    ProtonetCommandPtr protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, false, QByteArray(), 0, pressButtonCmd);
    ScpiObjectPtr scpiObject = m_scpiInterface->getSCPIObject(pressButtonCmd);
    ScpiDelegate* scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE(protoCmd->m_sOutput, ZSCPI::scpiAnswer[ZSCPI::ack]);
}

void test_emob_lock_button::readLockStateChannelUl1()
{
    QString cmd = "SYSTEM:EMOB:LOCKST? UL1;";
    ProtonetCommandPtr protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, false, QByteArray(), 0, cmd);
    ScpiObjectPtr scpiObject = m_scpiInterface->getSCPIObject(cmd);
    ScpiDelegate* scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE(protoCmd->m_sOutput, "0");
}

