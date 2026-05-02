#include "generatorchannelinterface.h"
#include "zscpi_response_definitions.h"

GeneratorChannelInterface::GeneratorChannelInterface(const std::shared_ptr<cSCPI> &scpiInterface,
                                   const cSenseSettingsPtr senseSettings,
                                   const SenseSystem::cChannelSettings *channelSettings,
                                   AbstractFactoryI2cCtrlPtr ctrlFactory) :
    ScpiConnection(scpiInterface),
    m_senseSettings(senseSettings),
    m_ctrlFactory(ctrlFactory),
    m_mName(channelSettings->m_nameMx),
    m_alias(channelSettings->m_sAlias1)
{
}

enum ScpiCommands {
    controllerSetRangeByAmplitude,
    controllerGetSetRange,
    dspGetSetAmplitude,
    dspGetSetFrequency,
    dspGetSetAngle,
};

void GeneratorChannelInterface::initSCPIConnection()
{
    const QString scpiLead = QString("GENERATOR:%1").arg(m_mName);
    // Do we need range lists?
    addDelegate(scpiLead, "AMPRANGE", SCPI::isCmdwP, m_scpiInterface, controllerSetRangeByAmplitude);
    addDelegate(scpiLead, "RANGE", SCPI::isQuery | SCPI::isCmdwP, m_scpiInterface, controllerGetSetRange);
    addDelegate(scpiLead, "DSAMPLITUDE", SCPI::isQuery | SCPI::isCmdwP, m_scpiInterface, dspGetSetAmplitude);
    addDelegate(scpiLead, "DSFREQUENCY", SCPI::isQuery | SCPI::isCmdwP, m_scpiInterface, dspGetSetFrequency);
    addDelegate(scpiLead, "DSANGLE", SCPI::isQuery | SCPI::isCmdwP, m_scpiInterface, dspGetSetAngle);
}

void GeneratorChannelInterface::executeProtoScpi(int cmdCode, const ProtonetCommandPtr &protoCmd)
{
    switch(cmdCode)
    {
    case controllerSetRangeByAmplitude:
        protoCmd->m_sOutput = scpiChangeRangeByAmplitude(protoCmd->m_sInput);
        break;
    case controllerGetSetRange:
        protoCmd->m_sOutput = scpiChangeRange(protoCmd->m_sInput);
        break;
    case dspGetSetAmplitude:
        protoCmd->m_sOutput = scpiDspAmplitude(protoCmd->m_sInput);
        break;
    case dspGetSetFrequency:
        protoCmd->m_sOutput = scpiDspFrequency(protoCmd->m_sInput);
        break;
    case dspGetSetAngle:
        protoCmd->m_sOutput = scpiDspAngle(protoCmd->m_sInput);
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
            if(controller->setRangeByAmplitude(m_mName, amplitude) == ZeraMControllerIo::cmddone) {
                emit sigMeasRangeProbablyChanged(m_mName);
                return ZSCPI::scpiAnswer[ZSCPI::ack];
            }
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
            if(controller->setRange(m_mName, range) == ZeraMControllerIo::cmddone) {
                emit sigMeasRangeProbablyChanged(m_mName);
                return  ZSCPI::scpiAnswer[ZSCPI::ack];
            }
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

QString GeneratorChannelInterface::scpiDspAmplitude(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    I2cCtrlGeneratorPtr controller = m_ctrlFactory->getGeneratorController(m_senseSettings);
    if (cmd.isCommand(1)) {
        bool ok;
        float amplitude = cmd.getParam(0).toFloat(&ok);
        if (ok) {
            if(controller->setDspAmplitude(m_mName, amplitude) == ZeraMControllerIo::cmddone)
                return  ZSCPI::scpiAnswer[ZSCPI::ack];
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
        }
    }
    else if(cmd.isQuery()) {
        float amplitude;
        if(controller->getDspAmplitude(m_mName, amplitude) == ZeraMControllerIo::cmddone)
            return QString::number(amplitude);
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString GeneratorChannelInterface::scpiDspFrequency(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    I2cCtrlGeneratorPtr controller = m_ctrlFactory->getGeneratorController(m_senseSettings);
    if (cmd.isCommand(1)) {
        bool ok;
        float frequency = cmd.getParam(0).toFloat(&ok);
        if (ok) {
            if(controller->setDspFrequency(m_mName, frequency) == ZeraMControllerIo::cmddone)
                return  ZSCPI::scpiAnswer[ZSCPI::ack];
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
        }
    }
    else if(cmd.isQuery()) {
        float frequency;
        if(controller->getDspFrequency(m_mName, frequency) == ZeraMControllerIo::cmddone)
            return QString::number(frequency);
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString GeneratorChannelInterface::scpiDspAngle(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    I2cCtrlGeneratorPtr controller = m_ctrlFactory->getGeneratorController(m_senseSettings);
    if (cmd.isCommand(1)) {
        bool ok;
        float angleDeg = cmd.getParam(0).toFloat(&ok);
        if (ok) {
            if(controller->setDspAngle(m_mName, angleDeg) == ZeraMControllerIo::cmddone)
                return  ZSCPI::scpiAnswer[ZSCPI::ack];
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
        }
    }
    else if(cmd.isQuery()) {
        float angleDeg;
        if(controller->getDspAngle(m_mName, angleDeg) == ZeraMControllerIo::cmddone)
            return QString::number(angleDeg);
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}
