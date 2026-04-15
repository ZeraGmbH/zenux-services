#include "generatorinterface.h"
#include "zera-jsonfileloader.h"
#include "zscpi_response_definitions.h"
#include "commonscpimethods.h"
#include <zera-json-params-structure.h>

GeneratorInterface::GeneratorInterface(std::shared_ptr<cSCPI> scpiInterface,
                                       SourceControlSettings *settings,
                                       cSenseSettingsPtr senseSettings,
                                       const QList<GeneratorChannelInterface *> &channels,
                                       AbstractFactoryI2cCtrlPtr ctrlFactory) :
    ScpiServerConnection(scpiInterface),
    m_sourceCapabilityFileName(settings->getSourceCapFile()),
    m_senseSettings(senseSettings),
    m_channels(channels),
    m_ctrlFactory(ctrlFactory)
{
}

enum ScpiCommands {
    sourceCapabilities,
    sourceModeOn,
    sourceOn,
};

void GeneratorInterface::initSCPIConnection()
{
    QJsonObject capabilities = expandJsonCapabilities(cJsonFileLoader::loadJsonFile(m_sourceCapabilityFileName));
    if (!capabilities.isEmpty()) {
        QJsonDocument docCapabilities(capabilities);
        m_sourceCapabilities = docCapabilities.toJson();
        addDelegate("GENERATOR", "CAPABILITIES", SCPI::isQuery, m_scpiInterface, sourceCapabilities);
    }
    addDelegate("GENERATOR", "MODEON", SCPI::isQuery | SCPI::isCmdwP, m_scpiInterface, sourceModeOn, &m_sourceOnModesNotification);
    addDelegate("GENERATOR", "SWITCHON", SCPI::isQuery | SCPI::isCmdwP, m_scpiInterface, sourceOn);

    for (GeneratorChannelInterface *channel : qAsConst(m_channels)) {
        channel->initSCPIConnection();
        connect(channel, &ScpiConnection::cmdExecutionDone,
                this, &ScpiConnection::cmdExecutionDone);
        connect(channel, &GeneratorChannelInterface::sigMeasRangeProbablyChanged,
                this, &GeneratorInterface::sigMeasRangeProbablyChanged);
    }
}

void GeneratorInterface::executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    switch (cmdCode)
    {
    case sourceCapabilities:
        if (cmd.isQuery())
            protoCmd->m_sOutput = m_sourceCapabilities;
        else
            protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
        break;
    case sourceModeOn:
        protoCmd->m_sOutput = scpiSourceModeOn(protoCmd->m_sInput);
        break;
    case sourceOn:
        protoCmd->m_sOutput = scpiSourceOn(protoCmd->m_sInput);
        break;
    }
    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

QString GeneratorInterface::scpiSourceModeOn(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    I2cCtrlGeneratorPtr controller = m_ctrlFactory->getGeneratorController(m_senseSettings);
    if (cmd.isCommand(1)) {
        const QString channelMNames = cmd.getParam(0);
        const QStringList channelMNameList = channelMNames.split(",", Qt::SkipEmptyParts);
        if (CommonScpiMethods::containsValidChannelMNames(m_senseSettings, channelMNameList) &&
            controller->sendSourceModeOn(channelMNameList) == ZeraMControllerIo::cmddone) {
            m_sourceOnModesNotification = channelMNames;
            return  ZSCPI::scpiAnswer[ZSCPI::ack];
        }
        return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    else if (cmd.isQuery()) {
        QStringList channelMNameList;
        if (controller->readSourceModeOn(channelMNameList) == ZeraMControllerIo::cmddone)
            return channelMNameList.join(",");
        return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString GeneratorInterface::scpiSourceOn(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    I2cCtrlGeneratorPtr controller = m_ctrlFactory->getGeneratorController(m_senseSettings);
    if (cmd.isCommand(1)) {
        const QStringList channelMNames = cmd.getParam(0).split(",", Qt::SkipEmptyParts);
        if (CommonScpiMethods::containsValidChannelMNames(m_senseSettings, channelMNames) &&
            controller->sendSourceOn(channelMNames) == ZeraMControllerIo::cmddone)
            return  ZSCPI::scpiAnswer[ZSCPI::ack];
        return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    else if (cmd.isQuery()) {
        QStringList channelMNames;
        if (controller->readSourceOn(channelMNames) == ZeraMControllerIo::cmddone)
            return channelMNames.join(",");
        return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QJsonObject GeneratorInterface::expandJsonCapabilities(const QJsonObject &capabilitiesRaw)
{
    ZeraJsonParamsStructure jsonParamsStructure;
    jsonParamsStructure.setJson(capabilitiesRaw);
    return jsonParamsStructure.getJson();
}
