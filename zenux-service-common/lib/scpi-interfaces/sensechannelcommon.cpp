#include "sensechannelcommon.h"
#include "zscpi_response_definitions.h"

enum Commands
{
    cmdAlias,
    cmdType,
    cmdUnit,
    cmdDspChannel,
    cmdRange,
    cmdUrvalue,
    cmdRangeCat,

    cmdStatus,
    cmdStatusReset,
    cmdHardOverLoad
};

SenseChannelCommon::SenseChannelCommon(const std::shared_ptr<cSCPI> &scpiInterface,
                                       const QString &unit,
                                       SenseSystem::cChannelSettings *cSettings,
                                       const AbstractFactoryI2cCtrlPtr &ctrlFactory) :
    ScpiConnection(scpiInterface),
    m_ctrlFactory(ctrlFactory),
    m_settings(*cSettings),
    m_sUnit(unit),
    m_nMMode(0)
{
    connect(&m_delayedCtrlIos, &AsyncQueuedMControllerIo::sigCmdDone, this,
            [this](const ProtonetCommandPtr &protoCmd, const QString &rangeName) {
        notifierSenseChannelRange = rangeName;
        emit cmdExecutionDone(protoCmd);
    });
}

SenseChannelCommon::~SenseChannelCommon()
{
    for(auto range : qAsConst(m_RangeList))
        delete range;
}

void SenseChannelCommon::setRangeList(const QList<SenseRangeCommon *> &list)
{
    m_RangeList = list;
    setNotifierSenseChannelRangeCat();
    refreshNotifierForActualRange();
}

const QList<SenseRangeCommon *> &SenseChannelCommon::getRangeList()
{
    return m_RangeList;
}

SenseRangeCommon *SenseChannelCommon::getRange(const QString &name)
{
    for(auto range : qAsConst(m_RangeList))
        if(range->getRangeName() == name)
            return range;
    return nullptr;
}

void SenseChannelCommon::addRangeList(const QList<SenseRangeCommon *> &list)
{
    for(auto range : list)
        m_RangeList.append(range);
    setNotifierSenseChannelRangeCat();
}

void SenseChannelCommon::removeRangeList(const QList<SenseRangeCommon *> &list)
{
    for(auto range : list)
        m_RangeList.removeOne(range);
    setNotifierSenseChannelRangeCat();
}

quint8 SenseChannelCommon::getAdjustmentStatus80Mask()
{
    quint8 adj = 255;
    for(auto range : qAsConst(m_RangeList))
        adj &= range->getAdjustmentStatus80Mask();
    return adj;
}

QString SenseChannelCommon::getName()
{
    return m_settings.m_nameMx;
}

quint8 SenseChannelCommon::getCtrlChannel()
{
    return m_settings.m_nCtrlChannel;
}

void SenseChannelCommon::setUnit(const QString &s)
{
    m_sUnit = s;
}

void SenseChannelCommon::initSCPIConnection(const QString &leadingNodes)
{
    const QString adjLeadNodes = appendTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1%2").arg(adjLeadNodes, m_settings.m_nameMx),"ALIAS", SCPI::isQuery, m_scpiInterface, cmdAlias);
    addDelegate(QString("%1%2").arg(adjLeadNodes, m_settings.m_nameMx),"TYPE", SCPI::isQuery, m_scpiInterface, cmdType);
    addDelegate(QString("%1%2").arg(adjLeadNodes, m_settings.m_nameMx),"UNIT", SCPI::isQuery, m_scpiInterface, cmdUnit);
    addDelegate(QString("%1%2").arg(adjLeadNodes, m_settings.m_nameMx),"DSPCHANNEL", SCPI::isQuery, m_scpiInterface, cmdDspChannel);
    addDelegate(QString("%1%2").arg(adjLeadNodes, m_settings.m_nameMx),"RANGE", SCPI::isQuery | SCPI::isCmdwP, m_scpiInterface, cmdRange, &notifierSenseChannelRange);
    addDelegate(QString("%1%2").arg(adjLeadNodes, m_settings.m_nameMx),"URVALUE", SCPI::isQuery, m_scpiInterface, cmdUrvalue);
    addDelegate(QString("%1%2:RANGE").arg(adjLeadNodes, m_settings.m_nameMx),"CATALOG", SCPI::isQuery, m_scpiInterface, cmdRangeCat, &notifierSenseChannelRangeCat);
    for(auto range : qAsConst(m_RangeList)) {
        connect(range, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
        range->initSCPIConnection(QString("%1%2").arg(adjLeadNodes, m_settings.m_nameMx));
    }

    // Old implementation
    addDelegate(QString("%1%2").arg(adjLeadNodes, m_settings.m_nameMx),"STATUS", SCPI::isQuery, m_scpiInterface, cmdStatus);
    addDelegate(QString("%1%2:STATUS").arg(adjLeadNodes, m_settings.m_nameMx),"RESET", SCPI::isCmd, m_scpiInterface, cmdStatusReset);
    // New implementation
    addDelegate(QString("%1%2").arg(adjLeadNodes, m_settings.m_nameMx),"HARDOVERLOAD", SCPI::isQuery, m_scpiInterface, cmdHardOverLoad);
}

void SenseChannelCommon::setMMode(int mode)
{
    m_nMMode = mode;
    for(auto range : qAsConst(m_RangeList))
        range->setMModeToUpdateAvailability(mode);
    notifierSenseChannelRangeCat.forceTrigger(); // better we would ask for changed avail ranges and then trigger !!!
    // but we can do this later
}

SenseChannelCommon::NotificationStatus SenseChannelCommon::setRangeCommon(SenseRangeCommon* range,
                                                                          const ProtonetCommandPtr &protoCmd)
{
    if ( range && range->getAvail() ) {
        I2cCtrlRangesPtr rangeCtrl = m_ctrlFactory->getRangesController();
        ZeraMControllerIo::atmelRM atmelCmdState = rangeCtrl->setRange(m_settings.m_nCtrlChannel, range->getSelCode());
        const QString rangeName = range->getRangeName();
        if (atmelCmdState == ZeraMControllerIo::cmddone) {
            notifierSenseChannelRange = rangeName;
            protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::ack];
            return SenseChannelCommon::NotificationNow;
        }
        else if (atmelCmdState == ZeraMControllerIo::cmdneedsasyncio) {
            m_delayedCtrlIos.startAsyncRangeIo(std::move(rangeCtrl), protoCmd, rangeName);
            return SenseChannelCommon::NotificationDelayed;
        }
        else {
            protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::errexec];
            return SenseChannelCommon::NotificationNow;
        }
    }
    protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
    return SenseChannelCommon::NotificationNow;
}

void SenseChannelCommon::executeProtoScpi(int cmdCode, const ProtonetCommandPtr &protoCmd)
{
    bool notifyHere = true;
    switch (cmdCode)
    {
    case cmdAlias:
        protoCmd->m_sOutput = scpiReadAlias(protoCmd->m_sInput);
        break;
    case cmdType:
        protoCmd->m_sOutput = scpiReadType(protoCmd->m_sInput);
        break;
    case cmdUnit:
        protoCmd->m_sOutput = scpiReadUnit(protoCmd->m_sInput);
        break;
    case cmdDspChannel:
        protoCmd->m_sOutput = scpiReadDspChannel(protoCmd->m_sInput);
        break;
    case cmdStatus:
        protoCmd->m_sOutput = scpiReadChannelStatus(protoCmd->m_sInput);
        break;
    case cmdStatusReset:
        protoCmd->m_sOutput = scpiStatusReset(protoCmd->m_sInput);
        break;
    case cmdHardOverLoad:
        protoCmd->m_sOutput = scpiReadHardOverload(protoCmd->m_sInput);
        break;
    case cmdRange:
        notifyHere = scpiReadWriteRange(protoCmd) == SenseChannelCommon::NotificationNow;
        break;
    case cmdUrvalue:
        protoCmd->m_sOutput = scpiReadUrvalue(protoCmd->m_sInput);
        break;
    case cmdRangeCat:
        protoCmd->m_sOutput = scpiReadRangeCatalog(protoCmd->m_sInput);
        break;
    }
    if (protoCmd->m_bwithOutput && notifyHere)
        emit cmdExecutionDone(protoCmd);
}

bool SenseChannelCommon::initJustData()
{
    bool atLeastOneComputed = false;
    for(auto range : qAsConst(m_RangeList)) {
        if(range->initJustData())
            atLeastOneComputed = true;
    }
    return atLeastOneComputed;
}

bool SenseChannelCommon::computeJustData()
{
    bool atLeastOneComputed = false;
    for(auto range : qAsConst(m_RangeList)) {
        if(range->computeJustData())
            atLeastOneComputed = true;
    }
    return atLeastOneComputed;
}

QString SenseChannelCommon::scpiReadAlias(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return getAlias();
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseChannelCommon::scpiReadType(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return QString("0");
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseChannelCommon::scpiReadUnit(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return m_sUnit;
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseChannelCommon::scpiReadDspChannel(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return QString("%1").arg(m_settings.m_nDspChannel);
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseChannelCommon::scpiReadChannelStatus(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery()) {
        quint16 status;
        if (m_ctrlFactory->getCriticalStatusController()->readCriticalStatus(status) == ZeraMControllerIo::cmddone ) {
            quint32 r = 0;
            if (m_settings.m_nOverloadBit >= 0) { // perhaps this channel has no overload bit
                if ( (status & (1 << m_settings.m_nOverloadBit))  > 0)
                    r |= 1;
            }
            return QString("%1").arg(r);
        }
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseChannelCommon::scpiStatusReset(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isCommand(1) && (cmd.getParam(0) == "")) {
        if (m_settings.m_nOverloadBit >= 0) {
            if (m_ctrlFactory->getCriticalStatusController()->resetCriticalStatus((quint16)(1 << m_settings.m_nOverloadBit)) == ZeraMControllerIo::cmddone )
                return ZSCPI::scpiAnswer[ZSCPI::ack];
            else
                return ZSCPI::scpiAnswer[ZSCPI::errexec];
        }
        else
            return ZSCPI::scpiAnswer[ZSCPI::ack];
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseChannelCommon::scpiReadHardOverload(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery()) {
        quint16 status;
        if (m_ctrlFactory->getCriticalStatusController()->readCriticalStatus(status) == ZeraMControllerIo::cmddone ) {
            if (m_settings.m_nOverloadBit >= 0) { // perhaps this channel has no overload bit
                if (status & (1 << m_settings.m_nOverloadBit))
                    return "1";
            }
            return "0";
        }
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseChannelCommon::scpiReadUrvalue(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery()) {
        for(auto range : qAsConst(m_RangeList))
            if (range->getRangeName() == notifierSenseChannelRange.getString())
                return QString("%1").arg(range->getUpperRangevalue());
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseChannelCommon::scpiReadRangeCatalog(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return notifierSenseChannelRangeCat.getString();
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

void SenseChannelCommon::setNotifierSenseChannelRangeCat()
{
    int i;
    QString s;
    for (i = 0; i < m_RangeList.count()-1; i++)
        s += (m_RangeList.at(i)->getRangeName() + ";");
    s += m_RangeList.at(i)->getRangeName();
    notifierSenseChannelRangeCat = s;
}
