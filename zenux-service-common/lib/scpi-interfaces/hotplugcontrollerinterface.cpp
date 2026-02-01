#include "hotplugcontrollerinterface.h"
#include "zscpi_response_definitions.h"

HotplugControllerInterface::HotplugControllerInterface(std::shared_ptr<cSCPI> scpiInterface,
                                                       HotPluggableControllerContainerPtr hotPluggableControllerContainer) :
    ScpiConnection(scpiInterface),
    m_hotPluggableControllerContainer(hotPluggableControllerContainer)
{
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
    const QString adjLeadNodes = appendTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1SYSTEM:EMOB").arg(adjLeadNodes), "PBPRESS", SCPI::isCmd, m_scpiInterface, cmdEmobPushButtonPress);
    addDelegate(QString("%1SYSTEM:EMOB").arg(adjLeadNodes), "LOCKST", SCPI::isQuery, m_scpiInterface, cmdEmobReadLockState);
    addDelegate(QString("%1SYSTEM:EMOB").arg(adjLeadNodes), "ERROR", SCPI::isQuery, m_scpiInterface, cmdEmobReadErrorStatus);
    addDelegate(QString("%1SYSTEM:EMOB").arg(adjLeadNodes), "CLEARERROR", SCPI::isCmd, m_scpiInterface, cmdEmobClearErrorStatus);
    addDelegate(QString("%1SYSTEM:EMOB").arg(adjLeadNodes), "READDATA", SCPI::isQuery, m_scpiInterface, cmdEmobReadData);
    addDelegate(QString("%1SYSTEM:EMOB").arg(adjLeadNodes), "WRITEDATA", SCPI::isCmd, m_scpiInterface, cmdEmobWriteData);
}

QByteArray HotplugControllerInterface::decodeHexString(const QString &encoded)
{
    QStringList hexReceived = encoded.split(",", Qt::SkipEmptyParts);
    QByteArray received;
    received.resize(hexReceived.count());
    for (int i=0; i<hexReceived.count(); ++i)
        received[i] = hexReceived[i].toUInt(nullptr, 16);
    return received;
}

QStringList HotplugControllerInterface::encodeDataToHex(const QByteArray &data)
{
    QStringList hexBytes;
    for (int byteNo=0; byteNo<data.size(); ++byteNo) {
        QString hexByte = QString("%1").arg(ushort(data[byteNo]), 0, 16); // hex output
        hexByte = QString("00" + hexByte).right(2);
        hexBytes.append(hexByte);
    }
    return hexBytes;
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
        protoCmd->m_sOutput = emobWriteDataForExchange(protoCmd->m_sInput);
        break;
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
            quint16 errorStatus = 0;
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

QString HotplugControllerInterface::emobReadDataForExchange(const QString &scpiCmd)
{
    cSCPICommand cmd = scpiCmd;
    if (cmd.isQuery(1)) {
        QString parameter = cmd.getParam(0);
        QString channelMNameFound = findEmobConnected(parameter);
        if (!channelMNameFound.isEmpty()) {
            HotControllerMap emobControllers = m_hotPluggableControllerContainer->getCurrentControllers();
            QByteArray dataReturned;
            ZeraMControllerIoTemplate::atmelRM ctrlRet = emobControllers[channelMNameFound].m_emobController->readExchangeData(dataReturned);
            if (ctrlRet == ZeraMControllerIo::cmddone) {
                QStringList hexBytes = encodeDataToHex(dataReturned);
                return hexBytes.join(",");
            }
        }
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString HotplugControllerInterface::emobWriteDataForExchange(const QString &scpiCmd)
{
    cSCPICommand cmd = scpiCmd;
    if (cmd.isCommand(2) || cmd.isCommand(3)) {
        QString channelMNameFound = findEmobConnected(cmd.getParam(0));
        int otherEmobId = cmd.getParam(1).toInt();
        QString dataToWriteHex = cmd.getParam(2);
        if (!channelMNameFound.isEmpty() && otherEmobId>=0 && otherEmobId<MaxHotControllerCount) {
            QByteArray dataToWrite = HotplugControllerInterface::decodeHexString(dataToWriteHex);
            HotControllerMap emobControllers = m_hotPluggableControllerContainer->getCurrentControllers();
            ZeraMControllerIoTemplate::atmelRM ctrlRet = emobControllers[channelMNameFound].m_emobController->writeExchangeData(dataToWrite, otherEmobId);
            if (ctrlRet != ZeraMControllerIo::cmddone)
                return ZSCPI::scpiAnswer[ZSCPI::errexec];
            return ZSCPI::scpiAnswer[ZSCPI::ack];
        }
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}
