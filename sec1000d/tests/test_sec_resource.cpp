#include "test_sec_resource.h"
#include "secdevicenodesingletonmock.h"
#include "scpisingletonfactory.h"
#include <timemachineobject.h>
#include <QTest>
#include <QList>

QTEST_MAIN(test_sec_resource)

static const char *setTwoResourcesCommand = "ECALCULATOR:SET 2;";
static const char *setFourResourcesCommand = "ECALCULATOR:SET 4;";
static const char *freeResourcesCommand = "ECALCULATOR:FREE";

void test_sec_resource::init()
{
    SecDeviceNodeSingletonMock::enableMock();
    m_xmlConfigReader = std::make_unique<Zera::XMLConfig::cReader>();
    m_xmlConfigReader->loadSchema(QStringLiteral(CONFIG_SOURCES_SEC1000D) + "/" + "sec1000d.xsd");
    m_ecalcSettings = std::make_unique<SecCalculatorSettings>(m_xmlConfigReader.get());
    connect(m_xmlConfigReader.get(),&Zera::XMLConfig::cReader::valueChanged,m_ecalcSettings.get(),&SecCalculatorSettings::configXMLInfo);
    m_inputSettings = std::make_unique<SecInputSettings>(m_xmlConfigReader.get());
    connect(m_xmlConfigReader.get(),&Zera::XMLConfig::cReader::valueChanged,m_inputSettings.get(),&SecInputSettings::configXMLInfo);
    m_xmlConfigReader->loadXMLFile(QStringLiteral(CONFIG_SOURCES_SEC1000D) + "/" + "sec1000d.xml");
    TimeMachineObject::feedEventLoop();
    m_secResource = std::make_unique<SecGroupResourceAndInterface>(m_ecalcSettings.get(), m_inputSettings.get(), nullptr);
    m_secResource->initSCPIConnection("");
}

void test_sec_resource::cleanup()
{
    m_secResource = nullptr;
    m_ecalcSettings = nullptr;
    m_inputSettings = nullptr;
    m_xmlConfigReader = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_sec_resource::setSecChannelsForAClient()
{
    cProtonetCommand protoCmd(0, false, false, QByteArray(), 0, setFourResourcesCommand);
    cSCPIObject* scpiObject = ScpiSingletonFactory::getScpiObj()->getSCPIObject(setFourResourcesCommand);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(&protoCmd);
    QCOMPARE(protoCmd.m_sOutput, "ec0;ec1;ec2;ec3;");
    QStringList channelsSet = protoCmd.m_sOutput.split(";");

    QList<SecChannel*> secChannels = m_secResource->getECalcChannelList();
    for(SecChannel *secCh : secChannels) {
        if(channelsSet.contains(secCh->getName()))
            QVERIFY(!secCh->isfree());
        else
            QVERIFY(secCh->isfree());
    }
}

void test_sec_resource::setAndFreeSecChannelsForAClient()
{
    cProtonetCommand protoCmd(0, false, false, QByteArray(), 0, setFourResourcesCommand);
    cSCPIObject* scpiObject = ScpiSingletonFactory::getScpiObj()->getSCPIObject(setFourResourcesCommand);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(&protoCmd);
    QCOMPARE(protoCmd.m_sOutput, "ec0;ec1;ec2;ec3;");

    protoCmd.m_sInput = freeResourcesCommand;
    scpiObject = ScpiSingletonFactory::getScpiObj()->getSCPIObject(freeResourcesCommand);
    scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(&protoCmd);
    QCOMPARE(protoCmd.m_sOutput, "ack");

    QList<SecChannel*> secChannels = m_secResource->getECalcChannelList();
    for(SecChannel *secCh : secChannels)
        QVERIFY(secCh->isfree());
}

void test_sec_resource::setSecChannelsForMultipleClientsFreeOneClient()
{
    cProtonetCommand protoCmd(0, true, false, QByteArray(1,'1'), 0, setTwoResourcesCommand);
    cSCPIObject* scpiObject = ScpiSingletonFactory::getScpiObj()->getSCPIObject(setTwoResourcesCommand);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(&protoCmd);
    QCOMPARE(protoCmd.m_sOutput, "ec0;ec1;");

    protoCmd.m_clientId = QByteArray(1, '2');
    scpiDelegate->executeSCPI(&protoCmd);
    QCOMPARE(protoCmd.m_sOutput, "ec2;ec3;");

    protoCmd.m_clientId = QByteArray(1, '3');
    protoCmd.m_sInput = setFourResourcesCommand;
    scpiObject = ScpiSingletonFactory::getScpiObj()->getSCPIObject(setFourResourcesCommand);
    scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(&protoCmd);
    QCOMPARE(protoCmd.m_sOutput, "ec4;ec5;ec6;ec7;");

    QStringList channelsToBeFreed = protoCmd.m_sOutput.split(";"); //set in previous command
    protoCmd.m_clientId = QByteArray(1, '3');
    protoCmd.m_sInput = freeResourcesCommand;
    scpiObject = ScpiSingletonFactory::getScpiObj()->getSCPIObject(freeResourcesCommand);
    scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(&protoCmd);
    QCOMPARE(protoCmd.m_sOutput, "ack");

    QList<SecChannel*> secChannels = m_secResource->getECalcChannelList();
    for(SecChannel *secCh : secChannels) {
        if(channelsToBeFreed.contains(secCh->getName()))
            QVERIFY(secCh->isfree());
        else
            QVERIFY(!secCh->isfree());
    }

}
