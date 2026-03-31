#include "generatorchannelinterface.h"
#include "zscpi_response_definitions.h"

GeneratorChannelInterface::GeneratorChannelInterface(std::shared_ptr<cSCPI> scpiinterface,
                                   const cSenseSettingsPtr senseSettings,
                                   const SenseSystem::cChannelSettings *channelSettings,
                                   AbstractFactoryI2cCtrlPtr ctrlFactory) :
    ScpiConnection(scpiinterface),
    m_senseSettings(senseSettings),
    m_ctrlFactory(ctrlFactory),
    m_mName(channelSettings->m_nameMx),
    m_alias(channelSettings->m_sAlias1)
{
}

enum ScpiCommands {
    sourceSetRangeByAmplitude,
    sourceGetSetRange
};

void GeneratorChannelInterface::initSCPIConnection()
{
    const QString scpiLead = QString("GENERATOR:%1").arg(m_mName);
    // Do we need range lists?
    addDelegate(scpiLead, "AMPRANGE", SCPI::isCmdwP, m_scpiInterface, sourceSetRangeByAmplitude);
    addDelegate(scpiLead, "RANGE", SCPI::isQuery | SCPI::isCmdwP, m_scpiInterface, sourceGetSetRange);
}

void GeneratorChannelInterface::executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd)
{
    switch(cmdCode)
    {
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

QString GeneratorChannelInterface::scpiChangeRangeByAmplitude(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    I2cCtrlGeneratorPtr controller = m_ctrlFactory->getGeneratorController(m_senseSettings);
    if (cmd.isCommand(1)) {
        bool ok;
        float amplitude = cmd.getParam(0).toFloat(&ok);
        if (ok) {
            if(controller->setRangeByAmplitude(m_mName, amplitude) == ZeraMControllerIo::cmddone)
                return ZSCPI::scpiAnswer[ZSCPI::ack];
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
        }
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString GeneratorChannelInterface::scpiChangeRange(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    I2cCtrlGeneratorPtr controller = m_ctrlFactory->getGeneratorController(m_senseSettings);
    if (cmd.isCommand(1)) {
        bool ok;
        quint8 range = cmd.getParam(0).toUInt(&ok);
        if (ok) {
            if(controller->setRange(m_mName, range) == ZeraMControllerIo::cmddone)
                return  ZSCPI::scpiAnswer[ZSCPI::ack];
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
        }
    }
    else if(cmd.isQuery()) {
        quint8 range;
        if(controller->readRange(m_mName, range) == ZeraMControllerIo::cmddone)
            return QString::number(range);
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}
