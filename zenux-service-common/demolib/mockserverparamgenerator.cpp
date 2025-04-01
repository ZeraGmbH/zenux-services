#include "mockserverparamgenerator.h"

ServerParams MockServerParamGenerator::createParams(const QString &serviceName)
{
    Q_INIT_RESOURCE(com5003d_config);
    Q_INIT_RESOURCE(mtXXXs2d_config);
    Q_INIT_RESOURCE(sec1000d_config);
    Q_INIT_RESOURCE(zdsp1d_config);

    const SettingsContainer::TServiceConfig config = SettingsContainer::getServiceConfig(serviceName);
    QString xsdName = ":/" + config.xsdFileName;
    QString xmlName = ":/" + config.xmlFileName;
    ServerParams params {
                        QStringLiteral("Mock") + serviceName,
        "V42.0",
        xsdName,
        xmlName
    };
    return params;
}
