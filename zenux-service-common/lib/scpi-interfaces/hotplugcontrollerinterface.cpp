#include "hotplugcontrollerinterface.h"
#include "zscpi_response_definitions.h"

HotplugControllerInterface::HotplugControllerInterface(std::shared_ptr<cSCPI> scpiInterface,
                                                       HotPluggableControllerContainerPtr hotPluggableControllerContainer) :
    ScpiConnection(scpiInterface),
    m_hotPluggableControllerContainer(hotPluggableControllerContainer)
{
    connect(hotPluggableControllerContainer.get(), &HotPluggableControllerContainer::sigControllersChanged,
            this, &HotplugControllerInterface::onControllersChanged);
}

enum HotplugCommands
{
    cmdEmobPushButtonPress,
    cmdEmobReadLockState,
    cmdEmobReadErrorStatus,
    cmdEmobClearErrorStatus,
    cmdEmobReadData,
    cmdEmobWriteData
};

void HotplugControllerInterface::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1SYSTEM:EMOB").arg(leadingNodes), "PBPRESS", SCPI::isCmd, m_scpiInterface, cmdEmobPushButtonPress);
    addDelegate(QString("%1SYSTEM:EMOB").arg(leadingNodes), "LOCKST", SCPI::isQuery, m_scpiInterface, cmdEmobReadLockState);
    addDelegate(QString("%1SYSTEM:EMOB").arg(leadingNodes), "ERROR", SCPI::isQuery, m_scpiInterface, cmdEmobReadErrorStatus);
    addDelegate(QString("%1SYSTEM:EMOB").arg(leadingNodes), "CLEARERROR", SCPI::isCmd, m_scpiInterface, cmdEmobClearErrorStatus);
    addDelegate(QString("%1SYSTEM:EMOB").arg(leadingNodes), "READDATA", SCPI::isQuery, m_scpiInterface, cmdEmobReadData);
    addDelegate(QString("%1SYSTEM:EMOB").arg(leadingNodes), "WRITEDATA", SCPI::isCmd, m_scpiInterface, cmdEmobWriteData);
}

void HotplugControllerInterface::onControllersChanged()
{

}

void HotplugControllerInterface::executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd)
{
    switch (cmdCode)
    {
    case cmdEmobPushButtonPress:
        protoCmd->m_sOutput = emobPushButtonPress(protoCmd->m_sInput);
        break;
    case cmdEmobReadLockState:
        protoCmd->m_sOutput = emobReadLockState(protoCmd->m_sInput);
        break;
    case cmdEmobReadErrorStatus:
        protoCmd->m_sOutput = emobReadErrorStatus(protoCmd->m_sInput);
        break;
    case cmdEmobClearErrorStatus:
        protoCmd->m_sOutput = emobClearErrorStatus(protoCmd->m_sInput);
        break;
    case cmdEmobReadData:
        protoCmd->m_sOutput = emobReadDataForExchange(protoCmd->m_sInput);
        break;
    case cmdEmobWriteData:
        protoCmd->m_sOutput = emobWriteData(protoCmd->m_sInput);
    }
    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

QString HotplugControllerInterface::emobPushButtonPress(const QString &scpiCmd)
{
    cSCPICommand cmd = scpiCmd;
    if (cmd.isCommand(1)) {
        HotControllerMap emobControllers = m_hotPluggableControllerContainer->getCurrentControllers();
        QString channelNameFound = findEmobConnected(cmd.getParam(0));
        if (!channelNameFound.isEmpty()) {
            ZeraMControllerIoTemplate::atmelRM ctrlRet = emobControllers[channelNameFound].m_emobController->sendPushbuttonPress();
            if (ctrlRet != ZeraMControllerIo::cmddone)
                return ZSCPI::scpiAnswer[ZSCPI::errexec];
            return ZSCPI::scpiAnswer[ZSCPI::ack];
        }
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString HotplugControllerInterface::emobReadLockState(const QString &scpiCmd)
{
    cSCPICommand cmd = scpiCmd;
    if (cmd.isQuery() || cmd.isQuery(1)) {
        HotControllerMap emobControllers = m_hotPluggableControllerContainer->getCurrentControllers();
        QString channelNameFound = findEmobConnected(cmd.getParam(0));
        if (!channelNameFound.isEmpty()) {
            quint8 state = 0;
            ZeraMControllerIoTemplate::atmelRM ctrlRet = emobControllers[channelNameFound].m_emobController->readEmobLockState(state);
            if (ctrlRet != ZeraMControllerIo::cmddone)
                return ZSCPI::scpiAnswer[ZSCPI::errexec];
            return QString::number(state);
        }
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString HotplugControllerInterface::emobReadErrorStatus(const QString &scpiCmd)
{
    cSCPICommand cmd = scpiCmd;
    if (cmd.isQuery() || cmd.isQuery(1)) {
        HotControllerMap emobControllers = m_hotPluggableControllerContainer->getCurrentControllers();
        QString channelNameFound = findEmobConnected(cmd.getParam(0));
        if (!channelNameFound.isEmpty()) {
            quint8 errorStatus = 0;
            ZeraMControllerIoTemplate::atmelRM ctrlRet = emobControllers[channelNameFound].m_emobController->readEmobErrorStatus(errorStatus);
            if (ctrlRet != ZeraMControllerIo::cmddone)
                return ZSCPI::scpiAnswer[ZSCPI::errexec];
            return QString::number(errorStatus);
        }
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString HotplugControllerInterface::emobClearErrorStatus(const QString &scpiCmd)
{
    cSCPICommand cmd = scpiCmd;
    if (cmd.isCommand(1)) {
        HotControllerMap emobControllers = m_hotPluggableControllerContainer->getCurrentControllers();
        QString channelNameFound = findEmobConnected(cmd.getParam(0));
        if (!channelNameFound.isEmpty()) {
            ZeraMControllerIoTemplate::atmelRM ctrlRet = emobControllers[channelNameFound].m_emobController->clearErrorStatus();
            if (ctrlRet != ZeraMControllerIo::cmddone)
                return ZSCPI::scpiAnswer[ZSCPI::errexec];
            return ZSCPI::scpiAnswer[ZSCPI::ack];
        }
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString HotplugControllerInterface::emobReadDataForExchange(const QString &scpiCmd)
{
    cSCPICommand cmd = scpiCmd;
    if(cmd.isQuery()) {
        HotControllerMap emobControllers = m_hotPluggableControllerContainer->getCurrentControllers();
        for(auto it = emobControllers.begin(); it != emobControllers.end(); it++) {
            QString channelMName = it.key();
            HotControllers controller = it.value();
            if(controller.m_controllerType == EmobControllerTypes::MT650e) {
                ZeraMControllerIoTemplate::atmelRM ctrlRet = emobControllers[channelMName].m_emobController->readData(m_data);
                if (ctrlRet != ZeraMControllerIo::cmddone)
                    return ZSCPI::scpiAnswer[ZSCPI::errexec];
                return QString(m_data);
            }
        }
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString HotplugControllerInterface::emobWriteData(const QString &scpiCmd)
{
    cSCPICommand cmd = scpiCmd;
    if(cmd.isCommand(1)) {
        QString parameter = cmd.getParam(0);
        HotControllerMap emobControllers = m_hotPluggableControllerContainer->getCurrentControllers();
        for(auto it = emobControllers.begin(); it != emobControllers.end(); it++) {
            QString channelMName = it.key();
            HotControllers controller = it.value();
            if(controller.m_controllerType == EmobControllerTypes::EMOB) {
                if(!parameter.isEmpty()) {
                    QByteArray ba;
                    ba.append(char(0x00));
                    ZeraMControllerIoTemplate::atmelRM ctrlRet = emobControllers[channelMName].m_emobController->writeData(ba);
                    if (ctrlRet != ZeraMControllerIo::cmddone)
                        return ZSCPI::scpiAnswer[ZSCPI::errexec];
                    return ZSCPI::scpiAnswer[ZSCPI::ack];
                }
                else if(!m_data.isEmpty()) {
                    ZeraMControllerIoTemplate::atmelRM ctrlRet = emobControllers[channelMName].m_emobController->writeData(m_data);
                    if (ctrlRet != ZeraMControllerIo::cmddone)
                        return ZSCPI::scpiAnswer[ZSCPI::errexec];
                    return ZSCPI::scpiAnswer[ZSCPI::ack];
                }
                m_data.clear();
            }
        }
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString HotplugControllerInterface::findEmobConnected(const QString &channelMNameScpiParam)
{
    HotControllerMap emobControllers = m_hotPluggableControllerContainer->getCurrentControllers();
    if (emobControllers.size() >= 1) {
        if(channelMNameScpiParam.isEmpty() && emobControllers.size() == 1)
            return emobControllers.firstKey();
        if(emobControllers.contains(channelMNameScpiParam))
            return channelMNameScpiParam;
    }
    return QString();
}
