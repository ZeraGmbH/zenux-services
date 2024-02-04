#include "test_sec_resource.h"
#include "mockfactorydevicenodesec.h"
#include "scpisingletonfactory.h"
#include "zscpi_response_definitions.h"
#include <timemachineobject.h>
#include <QTest>
#include <QList>

QTEST_MAIN(test_sec_resource)

static const char *setTwoResourcesCommand = "ECALCULATOR:SET 2;";
static const char *setFourResourcesCommand = "ECALCULATOR:SET 4;";
static const char *freeResourcesCommand = "ECALCULATOR:FREE";

void test_sec_resource::init()
{
    m_xmlConfigReader = std::make_unique<Zera::XMLConfig::cReader>();
    m_xmlConfigReader->loadSchema(QStringLiteral(CONFIG_SOURCES_SEC1000D) + "/" + "sec1000d.xsd");
    m_ecalcSettings = std::make_unique<SecCalculatorSettings>(m_xmlConfigReader.get());
    connect(m_xmlConfigReader.get(),&Zera::XMLConfig::cReader::valueChanged,m_ecalcSettings.get(),&SecCalculatorSettings::configXMLInfo);
    m_inputSettings = std::make_unique<SecInputSettings>(m_xmlConfigReader.get());
    connect(m_xmlConfigReader.get(),&Zera::XMLConfig::cReader::valueChanged,m_inputSettings.get(),&SecInputSettings::configXMLInfo);
    m_xmlConfigReader->loadXMLFile(QStringLiteral(CONFIG_SOURCES_SEC1000D) + "/" + "sec1000d.xml");
    TimeMachineObject::feedEventLoop();
    m_secResource = std::make_unique<SecGroupResourceAndInterface>(
        m_ecalcSettings.get(),
        m_inputSettings.get(),
        nullptr,
        std::make_shared<MockFactoryDeviceNodeSec>());
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

QString test_sec_resource::sendScpiCommand(XiQNetPeer *peer, QByteArray clientID, QString cmd)
{
    cProtonetCommand protoCmd(peer, false, false, clientID, 0, cmd);
    cSCPIObject* scpiObject = ScpiSingletonFactory::getScpiObj()->getSCPIObject(cmd);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(&protoCmd);
    return protoCmd.m_sOutput;
}

void test_sec_resource::setSecChannelsForAClient()
{
    QString returnString = sendScpiCommand(nullptr, QByteArray(), setFourResourcesCommand);
    QCOMPARE(returnString, "ec0;ec1;ec2;ec3;");
    QStringList channelsSet = returnString.split(";");

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
    sendScpiCommand(nullptr, QByteArray(), setFourResourcesCommand);
    QString returnString = sendScpiCommand(nullptr, QByteArray(), freeResourcesCommand);
    QCOMPARE(returnString, ZSCPI::scpiAnswer[ZSCPI::ack]);

    QList<SecChannel*> secChannels = m_secResource->getECalcChannelList();
    for(SecChannel *secCh : secChannels)
        QVERIFY(secCh->isfree());
}

void test_sec_resource::setSecChannelsForMultipleClientsFreeOneClient()
{
    QString returnString = sendScpiCommand(nullptr, QByteArray(1, '1'), setTwoResourcesCommand);
    QCOMPARE(returnString, "ec0;ec1;");

    returnString = sendScpiCommand(nullptr, QByteArray(1, '2'), setTwoResourcesCommand);
    QCOMPARE(returnString, "ec2;ec3;");

    returnString = sendScpiCommand(nullptr, QByteArray(1, '3'), setFourResourcesCommand);
    QCOMPARE(returnString, "ec4;ec5;ec6;ec7;");
    QStringList channelsToBeFreed = returnString.split(";");

    sendScpiCommand(nullptr, QByteArray(1, '3'), freeResourcesCommand);

    QList<SecChannel*> secChannels = m_secResource->getECalcChannelList();
    for(SecChannel *secCh : secChannels) {
        if(channelsToBeFreed.contains(secCh->getName()))
            QVERIFY(secCh->isfree());
        else
            QVERIFY(!secCh->isfree());
    }
}

void test_sec_resource::setSecChannelsForMultipleClientsOnePeerFreePeer()
{
    XiQNetPeer peer;
    sendScpiCommand(&peer, QByteArray(1, '1'), setFourResourcesCommand);
    sendScpiCommand(&peer, QByteArray(1, '2'), setFourResourcesCommand);
    QVERIFY(m_secResource->freeChannelsForThisPeer(&peer));
    QList<SecChannel*> secChannels = m_secResource->getECalcChannelList();
    for(SecChannel *secCh : secChannels)
        QVERIFY(secCh->isfree());
}

void test_sec_resource::setSecChannelsForMultipleClientsMultiplePeersFreeOnePeer()
{
    XiQNetPeer peer;
    sendScpiCommand(&peer, QByteArray(1, '1'), setTwoResourcesCommand);
    sendScpiCommand(&peer, QByteArray(1, '2'), setTwoResourcesCommand);

    XiQNetPeer peer1;
    QString channelsSet = sendScpiCommand(&peer1, QByteArray(1, '3'), setTwoResourcesCommand);
    channelsSet.append(sendScpiCommand(&peer1, QByteArray(1, '4'), setTwoResourcesCommand));

    QVERIFY(m_secResource->freeChannelsForThisPeer(&peer1));

    QStringList channelsToBeFreed = channelsSet.split(";");
    QList<SecChannel*> secChannels = m_secResource->getECalcChannelList();
    for(SecChannel *secCh : secChannels) {
        if(channelsToBeFreed.contains(secCh->getName()))
            QVERIFY(secCh->isfree());
        else
            QVERIFY(!secCh->isfree());
    }
}

void test_sec_resource::freeChannelsFromInvalidPeer()
{
    XiQNetPeer peer;
    sendScpiCommand(&peer, QByteArray(1, '1'), setTwoResourcesCommand);
    sendScpiCommand(&peer, QByteArray(1, '2'), setTwoResourcesCommand);

    XiQNetPeer peer1;
    QVERIFY(m_secResource->freeChannelsForThisPeer(&peer1)); //because no channels were set
}
