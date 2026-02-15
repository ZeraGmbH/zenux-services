#include "testserverforsenseinterfacemt310s2.h"
#include "clamp.h"
#include "mockclampplugger.h"
#include "mockeepromi2cfactory.h"
#include "mt310s2senseinterface.h"
#include "mt310s2systeminfomock.h"
#include <i2cmultiplexerfactory.h>
#include <testloghelpers.h>

TestServerForSenseInterfaceMt310s2::TestServerForSenseInterfaceMt310s2(AbstractFactoryI2cCtrlPtr ctrlFactory,
                                                                       VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                                                                       const QString &serviceNameForAlternateDevice,
                                                                       bool systemInfoMock) :
    TestPcbServer(serviceNameForAlternateDevice, tcpNetworkFactory)
{
    Q_INIT_RESOURCE(regression_data_adj_calc);
    PermissionFunctions::setPermissionCtrlFactory(ctrlFactory);
    if(systemInfoMock)
        m_systemInfo = std::make_unique<Mt310s2SystemInfoMock>(ctrlFactory);
    else
        m_systemInfo = std::make_unique<Mt310s2SystemInfo>(ctrlFactory);

    AbstractEepromI2cFactoryPtr adjMemFactory = std::make_shared<MockEepromI2cFactory>();
    const I2cSettingsPtr i2cSettings = m_settings->getI2cSettings();
    EepromI2cDeviceInterfacePtr adjEeprom = adjMemFactory->createEeprom(
        {i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress)},
        i2cSettings->getEepromByteSize());

    m_senseInterface = std::make_unique<Mt310s2SenseInterface>(m_scpiInterface,
                                                               m_settings->getSenseSettings(),
                                                               std::move(adjEeprom),
                                                               m_systemInfo.get(),
                                                               SettingsContainer::createChannelRangeFactory(serviceNameForAlternateDevice),
                                                               ctrlFactory);
    m_clampInterface = std::make_unique<cClampInterface>(this,
                                                         m_settings->getSenseSettings(),
                                                         m_senseInterface.get(),
                                                         i2cSettings,
                                                         adjMemFactory,
                                                         ctrlFactory);
    m_samplingInterface = std::make_unique<cSamplingInterface>(m_scpiInterface,
                                                               m_settings->getSamplingSettings(),
                                                               ctrlFactory);

    m_systemInterface = std::make_unique<Mt310s2SystemInterface>(this,
                                                                 m_systemInfo.get(),
                                                                 m_settings->getSenseSettings(),
                                                                 m_senseInterface.get(),
                                                                 ctrlFactory,
                                                                 nullptr);

    setScpiConnections(ScpiConnectionList{m_systemInterface.get(), m_clampInterface.get(), m_samplingInterface.get(), m_senseInterface.get()});

    start();
}

double TestServerForSenseInterfaceMt310s2::calcAdjValue(double value, double coeff0, double coeff1, double coeff2)
{
    return coeff0 + value*coeff1 + value*value*coeff2;
}

double TestServerForSenseInterfaceMt310s2::calcGainValue(double value)
{
    return calcAdjValue(value, 2.0, 3.0, 4.0); // matches xml
}

double TestServerForSenseInterfaceMt310s2::calcPhaseValue(double value)
{
    return calcAdjValue(value, 3.0, 4.0, 5.0); // matches xml
}

double TestServerForSenseInterfaceMt310s2::calcOffsetValue(double value)
{
    return calcAdjValue(value, 4.0, 5.0, 6.0); // matches xml
}

double TestServerForSenseInterfaceMt310s2::getCvRatioC200A()
{
    return 500.0;
}

double TestServerForSenseInterfaceMt310s2::calcGainValueClamp(double value)
{
    return calcAdjValue(value, 5.0, 6.0, 7.0); // matches xml
}

double TestServerForSenseInterfaceMt310s2::calcPhaseValueClamp(double value)
{
    return calcAdjValue(value, 6.0, 7.0, 8.0); // matches xml
}

double TestServerForSenseInterfaceMt310s2::calcOffsetValueClamp(double value)
{
    return calcAdjValue(value, 7.0, 8.0, 9.0); // matches xml
}

cClamp *TestServerForSenseInterfaceMt310s2::addClamp(int clampType, QString channelAlias)
{
    MockClampPlugger clampPlugger(getSenseSettings(), m_settings->getI2cSettings());
    clampPlugger.initClampAdjMem(clampType, channelAlias);

    SenseSystem::cChannelSettings *channelSettingClamps = getSenseSettings()->findChannelSettingByAlias1(channelAlias);
    return getClampInterface()->tryAddClamp(channelSettingClamps);
}

void TestServerForSenseInterfaceMt310s2::addStdClamp()
{
    addClamp(cClamp::CL200ADC1000VDC, "IL1");
    QString xml = TestLogHelpers::loadFile(":/import_clamp.xml");
    getClampInterface()->importClampXmls(xml, false);
}

void TestServerForSenseInterfaceMt310s2::removeAllClamps()
{
    // to execute as much production code as possible we use actualizeClampStatus
    getClampInterface()->actualizeClampStatus(0);
}
