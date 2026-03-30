#include "generatorinterface.h"
#include "zscpi_response_definitions.h"
#include "commonscpimethods.h"

GeneratorInterface::GeneratorInterface(std::shared_ptr<cSCPI> scpiInterface,
                                       cSenseSettingsPtr senseSettings,
                                       AbstractFactoryI2cCtrlPtr ctrlFactory) :
    ScpiServerConnection(scpiInterface),
    m_senseSettings(senseSettings),
    m_ctrlFactory(ctrlFactory)
{
}

enum ScpiCommands {
    sourceModeOn,
    sourceOn,
    sourceAmplitudeChangeRange
};

void GeneratorInterface::initSCPIConnection()
{
    addDelegate("GENERATOR", "MODEON", SCPI::isQuery | SCPI::isCmdwP, m_scpiInterface, sourceModeOn, &m_sourceOnModesNotification);
    addDelegate("GENERATOR", "SWITCHON", SCPI::isQuery | SCPI::isCmdwP, m_scpiInterface, sourceOn);
    addDelegate("GENERATOR", "AMPRANGE", SCPI::isCmdwP, m_scpiInterface, sourceAmplitudeChangeRange);
}

void GeneratorInterface::executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd)
{
    switch (cmdCode)
    {
    case sourceModeOn:
        protoCmd->m_sOutput = scpiSourceModeOn(protoCmd->m_sInput);
        break;
    case sourceOn:
        protoCmd->m_sOutput = scpiSourceOn(protoCmd->m_sInput);
        break;
    case sourceAmplitudeChangeRange:
        protoCmd->m_sOutput = scpiSourceAmplitudeChangeRange(protoCmd->m_sInput);
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

QString GeneratorInterface::scpiSourceAmplitudeChangeRange(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    I2cCtrlGeneratorPtr controller = m_ctrlFactory->getGeneratorController(m_senseSettings);
    if (cmd.isCommand(1)) {
        const QStringList paramList = cmd.getParam(0).split(",", Qt::SkipEmptyParts);
        if (paramList.size() == 2) {
            bool ok;
            float amplitude = paramList[0].toFloat(&ok);
            if (ok) {
                const QString channelMName = paramList[1];
                if (CommonScpiMethods::containsValidChannelMName(m_senseSettings, channelMName) &&
                    controller->setRangeByAmplitude(channelMName, amplitude) == ZeraMControllerIo::cmddone)
                    return  ZSCPI::scpiAnswer[ZSCPI::ack];
            }
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
        }
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}
