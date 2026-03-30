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
    sourceSetRangeByAmplitude,
    sourceGetSetRange
};

void GeneratorInterface::initSCPIConnection()
{
    // Due to multi channel commands we have no channels in contrast to measurement commands
    // Let's see if channels have to be introduced...
    addDelegate("GENERATOR", "MODEON", SCPI::isQuery | SCPI::isCmdwP, m_scpiInterface, sourceModeOn, &m_sourceOnModesNotification);
    addDelegate("GENERATOR", "SWITCHON", SCPI::isQuery | SCPI::isCmdwP, m_scpiInterface, sourceOn);
    addDelegate("GENERATOR", "AMPRANGE", SCPI::isCmdwP, m_scpiInterface, sourceSetRangeByAmplitude);
    addDelegate("GENERATOR", "RANGE", SCPI::isQuery | SCPI::isCmdwP, m_scpiInterface, sourceGetSetRange);
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
    case sourceSetRangeByAmplitude:
        protoCmd->m_sOutput = scpiChangeRangeByAmplitude(protoCmd->m_sInput);
        break;
    case sourceGetSetRange:
        protoCmd->m_sOutput = scpiChangeRange(protoCmd->m_sInput);
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

QString GeneratorInterface::scpiChangeRangeByAmplitude(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    I2cCtrlGeneratorPtr controller = m_ctrlFactory->getGeneratorController(m_senseSettings);
    if (cmd.isCommand(1)) {
        const QStringList paramList = cmd.getParam(0).split(",", Qt::SkipEmptyParts);
        if (paramList.size() == 2) {
            const QString channelMName = paramList[0];
            if (CommonScpiMethods::containsValidChannelMName(m_senseSettings, channelMName)) {
                bool ok;
                float amplitude = paramList[1].toFloat(&ok);
                if (ok) {
                    if(controller->setRangeByAmplitude(channelMName, amplitude) == ZeraMControllerIo::cmddone)
                        return ZSCPI::scpiAnswer[ZSCPI::ack];
                    return ZSCPI::scpiAnswer[ZSCPI::errexec];
                }
            }
        }
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString GeneratorInterface::scpiChangeRange(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    I2cCtrlGeneratorPtr controller = m_ctrlFactory->getGeneratorController(m_senseSettings);
    const QStringList paramList = cmd.getParam(0).split(",", Qt::SkipEmptyParts);
    if (cmd.isCommand(1) && paramList.size() == 2) {
        const QString channelMName = paramList[0];
        if (CommonScpiMethods::containsValidChannelMName(m_senseSettings, channelMName)) {
            bool ok;
            quint8 range = paramList[1].toUInt(&ok);
            if (ok) {
                if(controller->setRange(channelMName, range) == ZeraMControllerIo::cmddone)
                    return  ZSCPI::scpiAnswer[ZSCPI::ack];
                return ZSCPI::scpiAnswer[ZSCPI::errexec];
            }
        }
    }
    else if (cmd.isQuery(1) && paramList.size() == 1) {
        const QString channelMName = paramList[0];
        if (CommonScpiMethods::containsValidChannelMName(m_senseSettings, channelMName)) {
            quint8 range;
            if(controller->readRange(channelMName, range) == ZeraMControllerIo::cmddone)
                return QString::number(range);
        }
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}
