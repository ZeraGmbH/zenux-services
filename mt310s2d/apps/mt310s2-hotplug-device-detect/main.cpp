#include "settingscontainer.h"
#include "mt310s2dglobal.h"
#include "factoryi2cctrl.h"
#include "demofactoryi2cctrl.h"
#include "controllerpersitentdata.h"
#include "mockserverparamgenerator.h"
#include "i2cmuxerpca9547.h"
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>
#include <memory>

// A shell script helper for multiple firmware update
// It creates the file /tmp/hotplug-i2c-mux-info containing space separated
// PCA9547 muxer values for devices detected e.g for IL1,IL2,IL3,IAUX
// 0x09 0x0A 0x0B 0x0C

// Quick-hack-lazy:
static constexpr bool TESTMODE = false;

static SettingsContainerPtr getSettingsContainer(const QString &serviceName)
{
    SettingsContainer::TServiceConfig config = SettingsContainer::getServiceConfig(serviceName);
    const QString dirName = "/etc/zera/" + serviceName + "/";
    ServerParams defaultParams {
        8,
        ServerName,
        ServerVersion,
        dirName + config.xsdFileName,
        dirName + config.xmlFileName
    };
    if(TESTMODE)
        defaultParams = MockServerParamGenerator::createParams(serviceName);
    return std::make_unique<SettingsContainer>(defaultParams);
}

static std::unique_ptr<cSenseSettings> getSenseSettings(const QString &xmlFileName)
{
    Zera::XMLConfig::cReader m_xmlConfigReader;
    std::unique_ptr<cSenseSettings> senseSettings = std::make_unique<cSenseSettings>(&m_xmlConfigReader, 8);
    QObject::connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged, senseSettings.get(), &cSenseSettings::configXMLInfo);
    m_xmlConfigReader.loadXMLFile(xmlFileName);
    return senseSettings;
}

static AbstractFactoryI2cCtrlPtr getAndPrepareClampFactory(const QString &serviceName)
{
    std::unique_ptr<cSenseSettings> senseSettings = getSenseSettings(getSettingsContainer(serviceName)->getServerParams().getXmlFile());
    const QList<SenseSystem::cChannelSettings*>& channelSettings = senseSettings->getChannelSettings();
    if(TESTMODE) {
        AbstractFactoryI2cCtrlPtr ctrlFactory = std::make_shared<DemoFactoryI2cCtrl>(getSettingsContainer(serviceName));
        for (const SenseSystem::cChannelSettings* channelSetting : channelSettings) {
            if (channelSetting->m_nPluggedBit >= 0 && channelSetting->m_nMuxChannelNo >= 0)
                ControllerPersitentData::getData().m_clampConnectMask |= (1<<channelSetting->m_nPluggedBit);
        }
        return ctrlFactory;
    }
    return std::make_shared<FactoryI2cCtrl>(getSettingsContainer(serviceName)->getI2cSettings());
}

static QStringList getI2cMuxerValues(const QString &serviceName)
{
    AbstractFactoryI2cCtrlPtr ctrlFactory = getAndPrepareClampFactory(serviceName);
    I2cCtrlClampStatusPtr clampController = ctrlFactory->getClampStatusController();
    QStringList muxerStrings;
    QStringList channelAliasStrings;
    quint16 bitmaskAvailable;
    if (clampController->readClampStatus(bitmaskAvailable) == ZeraMControllerIo::cmddone) {
        std::unique_ptr<cSenseSettings> senseSettings = getSenseSettings(getSettingsContainer(serviceName)->getServerParams().getXmlFile());
        const QList<SenseSystem::cChannelSettings*>& channelSettings = senseSettings->getChannelSettings();
        for (const SenseSystem::cChannelSettings* channelSetting : channelSettings) {
            if (bitmaskAvailable & (1<<channelSetting->m_nPluggedBit)) {
                uchar muxByte = I2cMuxerPCA9547::calcControlByte(channelSetting->m_nMuxChannelNo);
                QString muxStr = QString("%1").arg(muxByte, 2, 16, QLatin1Char('0'));
                muxerStrings.append("0x" + muxStr.toUpper());
                channelAliasStrings.append(channelSetting->m_sAlias1);
            }
        }
    }
    if(channelAliasStrings.isEmpty())
        printf("No hotplugged devices detected.\n");
    else
        printf("Hotplugged devices on channels %s detected.\n", qPrintable(channelAliasStrings.join(",")));
    return muxerStrings;
}

bool isDeviceSupported(const QString &deviceName)
{
    const QStringList supportedDevices = QStringList() << "mt310s2" << "mt581s2";
    if (!supportedDevices.contains(deviceName)) {
        fprintf(stderr, "Invalid device name '%s' - supported are: %s!\n",
                qPrintable(deviceName), qPrintable(supportedDevices.join(" / ")));
        return false;
    }
    return true;
}

int main( int argc, char *argv[] )
{
    QCoreApplication app(argc, argv);
    QCommandLineParser parser;
    const QCommandLineOption deviceparam("d", "device", "Device name");
    parser.addOption(deviceparam);
    parser.process(app);
    QString serviceName = "mt310s2d";
    if (parser.isSet(deviceparam)) {
        QString deviceName = parser.value(deviceparam);
        if (!isDeviceSupported(deviceName))
            return -1;
        serviceName = deviceName + "d";
    }

    QStringList muxerStrings = getI2cMuxerValues(serviceName);
    QFile file("/tmp/hotplug-i2c-mux-info");
    if(file.open(QFile::WriteOnly)) {
        file.write(muxerStrings.join(" ").toLocal8Bit());
        file.close();
    }
    return 0;
}
