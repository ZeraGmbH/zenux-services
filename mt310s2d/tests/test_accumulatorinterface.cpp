#include "test_accumulatorinterface.h"
#include "mockfactoryi2cctrl.h"
#include <timerfactoryqtfortest.h>
#include <timemachinefortest.h>
#include <QTest>

QTEST_MAIN(test_accumulatorinterface);

static const char *systemAccumulatorStatus ="SYSTEM:ACCUMULATOR:STATUS?";
static const char *systemAccumulatorSoc ="SYSTEM:ACCUMULATOR:SOC?";

void test_accumulatorinterface::init()
{
    TimerFactoryQtForTest::enableTest();
    m_scpiInterface = std::make_unique<cSCPI>();

    m_xmlConfigReader = std::make_unique<Zera::XMLConfig::cReader>();
    m_accuSettings = std::make_unique<AccumulatorSettings>(m_xmlConfigReader.get());
    connect(m_xmlConfigReader.get(), &Zera::XMLConfig::cReader::valueChanged,
            m_accuSettings.get(), &AccumulatorSettings::configXMLInfo);
    m_xmlConfigReader->loadSchema(QStringLiteral(CONFIG_SOURCES_MT310S2D) + "/" + "mt310s2d.xsd");
    m_xmlConfigReader->loadXMLFile(QStringLiteral(CONFIG_SOURCES_MT310S2D) + "/" + "mt310s2d.xml");
    
    m_accumulator = new AccumulatorInterface(m_scpiInterface.get(), m_accuSettings.get(), std::make_shared<MockFactoryI2cCtrl>(true));
    m_accumulator->initSCPIConnection("");
}

void test_accumulatorinterface::cleanup()
{
    delete m_accumulator;
}

void test_accumulatorinterface::findObjects()
{
    QVERIFY(m_scpiInterface->getSCPIObject(systemAccumulatorStatus));
    QVERIFY(m_scpiInterface->getSCPIObject(systemAccumulatorSoc));
}

void test_accumulatorinterface::readAccumulatorStatus()
{
    TimeMachineForTest::getInstance()->processTimers(1500);
    std::unique_ptr<cProtonetCommand> protoCmd = std::make_unique<cProtonetCommand>(nullptr, false, false, QByteArray(), 0, systemAccumulatorStatus);
    cSCPIObject* scpiObject = m_scpiInterface->getSCPIObject(systemAccumulatorStatus);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd.get());
    if(m_accuSettings->isAvailable())
        QCOMPARE(protoCmd->m_sOutput, "0");
    else
        QCOMPARE(protoCmd->m_sOutput, "");
}

void test_accumulatorinterface::readAccuStateOfCharge()
{
    TimeMachineForTest::getInstance()->processTimers(1500);
    std::unique_ptr<cProtonetCommand> protoCmd = std::make_unique<cProtonetCommand>(nullptr, false, false, QByteArray(), 0, systemAccumulatorSoc);
    cSCPIObject* scpiObject = m_scpiInterface->getSCPIObject(systemAccumulatorSoc);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd.get());
    if(m_accuSettings->isAvailable())
        QCOMPARE(protoCmd->m_sOutput, "37");
    else
        QCOMPARE(protoCmd->m_sOutput, "");
}
