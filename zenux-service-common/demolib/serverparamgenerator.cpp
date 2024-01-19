#include "serverparamgenerator.h"

ServerParams ServerParamGenerator::createParams(QString serviceName)
{
    QString xsdName = ":/" + serviceName + ".xsd";
    QString xmlName = ":/" + serviceName + ".xml";
    if(serviceName == "com5003d")
        Q_INIT_RESOURCE(com5003d_config);
    else if(serviceName == "mt310s2d")
        Q_INIT_RESOURCE(mt310s2d_config);
    else if(serviceName == "sec1000d")
        Q_INIT_RESOURCE(sec1000d_config);
    else if(serviceName == "zdsp1d") // ooh that is far from mockable yet...
        Q_INIT_RESOURCE(zdsp1d_config);
    else
        qFatal("Cannot mock service: %s", qPrintable(serviceName));
    ServerParams params {
        QStringLiteral("Mock") + serviceName,
        "V42.0",
        xsdName,
        xmlName
    };
    return params;
}
