#include "mockserverparamgenerator.h"

ServerParams MockServerParamGenerator::createParams(const QString &serviceOrDeviceName,
                                                    const QString &alternateConfigXml)
{
    Q_INIT_RESOURCE(com5003d_config);
    Q_INIT_RESOURCE(mtXXXs2d_config);
    Q_INIT_RESOURCE(sec1000d_config);
    Q_INIT_RESOURCE(zdsp1d_config);

    const SettingsContainer::TServiceConfig config = SettingsContainer::getServiceConfig(serviceOrDeviceName);
    QString xsdName = ":/" + config.xsdFileName;
    QString xmlName = !alternateConfigXml.isEmpty() ? alternateConfigXml : ":/" + config.xmlFileName;
    int channelCount = 0;
    if(serviceOrDeviceName.toLower().contains("com5003"))
        channelCount = 6;
    else if(serviceOrDeviceName.toLower().contains("mt"))
        channelCount = 8;
    ServerParams params {
        channelCount,
        QStringLiteral("Mock") + serviceOrDeviceName,
        "V42.0",
        xsdName,
        xmlName
    };
    return params;
}
