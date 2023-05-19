#include "test_hotpluggablecontrollercontainer.h"
#include "atmelctrlfactoryfortest.h"
#include "atmelemobctrlfortest.h"
#include "hotpluggablecontrollercontainer.h"
#include <QTest>

QTEST_MAIN(test_hotpluggablecontrollercontainer);

test_hotpluggablecontrollercontainer::test_hotpluggablecontrollercontainer() :
    m_senseSettings(&m_configReader, 8 /*mt310s2*/)
{
    connect(&m_configReader, &Zera::XMLConfig::cReader::valueChanged,
            &m_senseSettings, &cSenseSettings::configXMLInfo);
    m_configReader.loadSchema(QStringLiteral(CONFIG_SOURCES_MT310S2D) + "/" + "mt310s2d.xsd");
    m_configReader.loadXMLFile(QStringLiteral(CONFIG_SOURCES_MT310S2D) + "/" + "mt310s2d.xml");
}

void test_hotpluggablecontrollercontainer::initTestCase()
{
    AtmelCtrlFactoryForTest::enableTest();
}

void test_hotpluggablecontrollercontainer::initNoController()
{
    HotPluggableControllerContainer container(QString(), 0, 0, 0);
    QVector<AtmelCommonVersionsPtr> controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 0);
}

void test_hotpluggablecontrollercontainer::mt310s2AllVoltageNotPluggable()
{
    HotPluggableControllerContainer container(QString(), 0, 0, 0);
    container.startActualizeEmobControllers(&m_senseSettings, (1<<0));
    container.startActualizeEmobControllers(&m_senseSettings, (1<<1));
    container.startActualizeEmobControllers(&m_senseSettings, (1<<2));
    container.startActualizeEmobControllers(&m_senseSettings, (1<<3));
    QVector<AtmelCommonVersionsPtr> controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 0);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1()
{
    HotPluggableControllerContainer container(QString(), 0, 0, 0);
    container.startActualizeEmobControllers(&m_senseSettings, (1<<4));
    QVector<AtmelCommonVersionsPtr> controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 1);

}

void test_hotpluggablecontrollercontainer::mt310s2AddI1I2()
{
    HotPluggableControllerContainer container(QString(), 0, 0, 0);
    container.startActualizeEmobControllers(&m_senseSettings, (1<<4) | (1<<5));
    QVector<AtmelCommonVersionsPtr> controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 2);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1Twice()
{
    HotPluggableControllerContainer container(QString(), 0, 0, 0);
    container.startActualizeEmobControllers(&m_senseSettings, (1<<4));
    container.startActualizeEmobControllers(&m_senseSettings, (1<<4));
    QVector<AtmelCommonVersionsPtr> controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 1);
    QCOMPARE(AtmelEmobCtrlForTest::getInstanceCount(), 1);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1I2AddI1()
{
    HotPluggableControllerContainer container(QString(), 0, 0, 0);
    container.startActualizeEmobControllers(&m_senseSettings, (1<<4) | (1<<5));
    container.startActualizeEmobControllers(&m_senseSettings, (1<<4));
    QVector<AtmelCommonVersionsPtr> controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 1);
    QCOMPARE(AtmelEmobCtrlForTest::getInstanceCount(), 1);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1CheckI2cSettings()
{
    HotPluggableControllerContainer container("foo", 1, 2, 3);
    container.startActualizeEmobControllers(&m_senseSettings, (1<<4));
    QVector<AtmelCommonVersionsPtr> controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 1);
    AtmelEmobCtrlForTest* ctrl = static_cast<AtmelEmobCtrlForTest*>(controllers[0].get());
    QCOMPARE(ctrl->getDevnode(), "foo");
    QCOMPARE(ctrl->getAdrCtrl(), 1);
    QCOMPARE(ctrl->getAdrMux(), 2);
    QCOMPARE(ctrl->getDebuglevel(), 3);
}

void test_hotpluggablecontrollercontainer::mt310s2AddI1I2I3IAuxCheckMuxSettings()
{
    HotPluggableControllerContainer container("foo", 1, 2, 3);
    container.startActualizeEmobControllers(&m_senseSettings, (1<<4) | (1<<5) | (1<<6) | (1<<7));
    QVector<AtmelCommonVersionsPtr> controllers = container.getCurrentControllers();
    QCOMPARE(controllers.size(), 4);
    AtmelEmobCtrlForTest* ctrlI1 = static_cast<AtmelEmobCtrlForTest*>(controllers[0].get());
    QCOMPARE(ctrlI1->getMuxChannel(), 1);
    AtmelEmobCtrlForTest* ctrlI2 = static_cast<AtmelEmobCtrlForTest*>(controllers[1].get());
    QCOMPARE(ctrlI2->getMuxChannel(), 2);
    AtmelEmobCtrlForTest* ctrlI3 = static_cast<AtmelEmobCtrlForTest*>(controllers[2].get());
    QCOMPARE(ctrlI3->getMuxChannel(), 3);
    AtmelEmobCtrlForTest* ctrlIAux = static_cast<AtmelEmobCtrlForTest*>(controllers[3].get());
    QCOMPARE(ctrlIAux->getMuxChannel(), 4);
}
