#include "sensechannelcommon.h"
#include "zscpi_response_definitions.h"

enum Commands
{
    cmdAlias,
    cmdType,
    cmdUnit,
    cmdDspChannel,
    cmdStatus,
    cmdStatusReset,
    cmdRange,
    cmdUrvalue,
    cmdRangeCat
};

SenseChannelCommon::SenseChannelCommon(std::shared_ptr<cSCPI> scpiinterface,
                                       const QString &unit,
                                       SenseSystem::cChannelSettings *cSettings,
                                       quint8 nr,
                                       AbstractFactoryI2cCtrlPtr ctrlFactory) :
    ScpiConnection(scpiinterface),
    m_ctrlFactory(ctrlFactory),
    m_sName(QString("m%1").arg(nr)),
    m_sAlias1(cSettings->m_sAlias1),
    m_sAlias2(cSettings->m_sAlias2),
    m_sUnit(unit),
    m_nCtrlChannel(cSettings->m_nCtrlChannel),
    m_nDspChannel(cSettings->m_nDspChannel),
    m_nOverloadBit(cSettings->m_nOverloadBit),
    m_bAvail(cSettings->avail),
    m_nMMode(0)
{
    connect(&m_delayedCtrlIos, &DelayedZeraMControllerKeepAliveContainer::sigCmdDone, this,
            [this](ProtonetCommandPtr protoCmd, QString rangeName) {
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
    setNotifierSenseChannelRange();
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
    return m_sName;
}

quint8 SenseChannelCommon::getCtrlChannel()
{
    return m_nCtrlChannel;
}

void SenseChannelCommon::setUnit(const QString &s)
{
    m_sUnit = s;
}

bool SenseChannelCommon::isAvail()
{
    return m_bAvail;
}

void SenseChannelCommon::initJustData()
{
    for(auto range : qAsConst(m_RangeList))
        range->initJustData();
}

void SenseChannelCommon::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName),"ALIAS", SCPI::isQuery, m_scpiInterface, cmdAlias);
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName),"TYPE", SCPI::isQuery, m_scpiInterface, cmdType);
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName),"UNIT", SCPI::isQuery, m_scpiInterface, cmdUnit);
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName),"DSPCHANNEL", SCPI::isQuery, m_scpiInterface, cmdDspChannel);
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName),"STATUS", SCPI::isQuery, m_scpiInterface, cmdStatus);
    addDelegate(QString("%1%2:STATUS").arg(leadingNodes, m_sName),"RESET", SCPI::isCmd, m_scpiInterface, cmdStatusReset);
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName),"RANGE", SCPI::isQuery | SCPI::isCmdwP, m_scpiInterface, cmdRange, &notifierSenseChannelRange);
    addDelegate(QString("%1%2").arg(leadingNodes, m_sName),"URVALUE", SCPI::isQuery, m_scpiInterface, cmdUrvalue);
    addDelegate(QString("%1%2:RANGE").arg(leadingNodes, m_sName),"CATALOG", SCPI::isQuery, m_scpiInterface, cmdRangeCat, &notifierSenseChannelRangeCat);
    for(auto range : qAsConst(m_RangeList)) {
        connect(range, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
        range->initSCPIConnection(QString("%1%2").arg(leadingNodes, m_sName));
    }
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
                                                                          ProtonetCommandPtr protoCmd)
{
    if ( range && range->getAvail() ) {
        I2cCtrlRangesPtr rangeCtrl = m_ctrlFactory->getRangesController();
        ZeraMControllerIo::atmelRM atmelCmdState = rangeCtrl->setRange(m_nCtrlChannel, range->getSelCode());
        const QString rangeName = range->getRangeName();
        if (atmelCmdState == ZeraMControllerIo::cmddone) {
            notifierSenseChannelRange = rangeName;
            protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::ack];
            return SenseChannelCommon::NotificationNow;
        }
        else if (atmelCmdState == ZeraMControllerIo::cmdneedsasyncio) {
            m_delayedCtrlIos.addPendingIo(std::move(rangeCtrl), protoCmd, rangeName);
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

void SenseChannelCommon::executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd)
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

void SenseChannelCommon::computeJustData()
{
    for(auto range : qAsConst(m_RangeList))
        range->computeJustData();
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
        return QString("%1").arg(m_nDspChannel);
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseChannelCommon::scpiReadChannelStatus(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery()) {
        quint16 status;
        if (m_ctrlFactory->getCriticalStatusController()->readCriticalStatus(status) == ZeraMControllerIo::cmddone ) {
            quint32 r = ((m_bAvail) ? 0 : 1 << 31);
            if (m_nOverloadBit >= 0) { // perhaps this channel has no overload bit
                if ( (status & (1 << m_nOverloadBit))  > 0)
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
        if (m_nOverloadBit >= 0) {
            if (m_ctrlFactory->getCriticalStatusController()->resetCriticalStatus((quint16)(1 << m_nOverloadBit)) == ZeraMControllerIo::cmddone )
                return ZSCPI::scpiAnswer[ZSCPI::ack];
            else
                return ZSCPI::scpiAnswer[ZSCPI::errexec];
        }
        else
            return ZSCPI::scpiAnswer[ZSCPI::ack];
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
