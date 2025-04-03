#include "mockserverparamgenerator.h"

ServerParams MockServerParamGenerator::createParams(const QString &serviceNameForAlternateDevice)
{
    Q_INIT_RESOURCE(com5003d_config);
    Q_INIT_RESOURCE(mtXXXs2d_config);
    Q_INIT_RESOURCE(sec1000d_config);
    Q_INIT_RESOURCE(zdsp1d_config);

    const SettingsContainer::TServiceConfig config = SettingsContainer::getServiceConfig(serviceNameForAlternateDevice);
    QString xsdName = ":/" + config.xsdFileName;
    QString xmlName = ":/" + config.xmlFileName;
    ServerParams params {
                        QStringLiteral("Mock") + serviceNameForAlternateDevice,
        "V42.0",
        xsdName,
        xmlName
    };
    return params;
}
