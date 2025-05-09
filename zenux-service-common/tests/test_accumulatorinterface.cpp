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
    QTest::newRow("MT310s2") << QString("mt310s2d");
    QTest::newRow("MT581s2") << QString("mt581s2d");
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
    ProtonetCommandPtr protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, false, QByteArray(), 0, systemAccumulatorStatus);
    ScpiObjectPtr scpiObject = m_scpiInterface->getSCPIObject(systemAccumulatorStatus);
    ScpiDelegate* scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
    scpiDelegate->executeSCPI(protoCmd);
    if(m_accuSettings->isAvailable())
        QCOMPARE(protoCmd->m_sOutput, "0");
    else
        QCOMPARE(protoCmd->m_sOutput, "");
}

void test_accumulatorinterface::readAccuStateOfCharge()
{
    TimeMachineForTest::getInstance()->processTimers(1500);
    ProtonetCommandPtr protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, false, QByteArray(), 0, systemAccumulatorSoc);
    ScpiObjectPtr scpiObject = m_scpiInterface->getSCPIObject(systemAccumulatorSoc);
    ScpiDelegate* scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
    scpiDelegate->executeSCPI(protoCmd);
    if(m_accuSettings->isAvailable())
        QCOMPARE(protoCmd->m_sOutput, "37");
    else
        QCOMPARE(protoCmd->m_sOutput, "");
}
