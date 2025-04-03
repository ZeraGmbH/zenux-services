#include "test_accumulatorinterface.h"
#include "settingscontainer.h"
#include "testfactoryi2cctrl.h"
#include <timerfactoryqtfortest.h>
#include <timemachinefortest.h>
#include <QTest>

QTEST_MAIN(test_accumulatorinterface);

static const char *systemAccumulatorStatus ="SYSTEM:ACCUMULATOR:STATUS?";
static const char *systemAccumulatorSoc ="SYSTEM:ACCUMULATOR:SOC?";

void test_accumulatorinterface::initTestCase_data()
{
    QTest::addColumn<QString>("serviceNameForAlternateDevice");
    QTest::newRow("mt310s2d") << QString("mt310s2d");
    QTest::newRow("mt581s2d") << QString("mt581s2d");
}

void test_accumulatorinterface::init()
{
    TimerFactoryQtForTest::enableTest();
    m_scpiInterface = std::make_shared<cSCPI>();

    m_xmlConfigReader = std::make_unique<Zera::XMLConfig::cReader>();
    m_accuSettings = std::make_unique<AccumulatorSettings>(m_xmlConfigReader.get());
    connect(m_xmlConfigReader.get(), &Zera::XMLConfig::cReader::valueChanged,
            m_accuSettings.get(), &AccumulatorSettings::configXMLInfo);
    QFETCH_GLOBAL(QString, serviceNameForAlternateDevice);
    SettingsContainer::TServiceConfig config = SettingsContainer::getServiceConfig(serviceNameForAlternateDevice);
    m_xmlConfigReader->loadSchema(QStringLiteral(CONFIG_SOURCES_MT310S2D) + "/" + config.xsdFileName);
    m_xmlConfigReader->loadXMLFile(QStringLiteral(CONFIG_SOURCES_MT310S2D) + "/" + config.xmlFileName);
    
    m_accumulator = new AccumulatorInterface(m_scpiInterface,
                                             m_accuSettings.get(),
                                             std::make_shared<TestFactoryI2cCtrl>(true));
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

void test_accumulatorinterface::readAccuStatus()
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
