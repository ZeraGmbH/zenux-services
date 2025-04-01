#include "mockserverparamgenerator.h"

ServerParams MockServerParamGenerator::createParams(const QString &nameForConfigAndRanges)
{
    Q_INIT_RESOURCE(com5003d_config);
    Q_INIT_RESOURCE(mtXXXs2d_config);
    Q_INIT_RESOURCE(sec1000d_config);
    Q_INIT_RESOURCE(zdsp1d_config);

    const SettingsContainer::TServiceConfig config = SettingsContainer::getServiceConfig(nameForConfigAndRanges);
    QString xsdName = ":/" + config.xsdFileName;
    QString xmlName = ":/" + config.xmlFileName;
    ServerParams params {
                        QStringLiteral("Mock") + nameForConfigAndRanges,
        "V42.0",
        xsdName,
        xmlName
    };
    return params;
}
