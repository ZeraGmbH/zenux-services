#include "sensechannelcommon.h"
#include "zscpi_response_definitions.h"

SenseChannelCommon::SenseChannelCommon(cSCPI *scpiinterface,
                                       QString description,
                                       QString unit,
                                       SenseSystem::cChannelSettings *cSettings,
                                       quint8 nr,
                                       AbstractFactoryI2cCtrlPtr ctrlFactory) :
    ScpiConnection(scpiinterface),
    m_ctrlFactory(ctrlFactory),
    m_sDescription(description),
    m_sUnit(unit)
{
    m_sName = QString("m%1").arg(nr);
    m_sAlias1 = cSettings->m_sAlias1;
    m_sAlias2 = cSettings->m_sAlias2;
    m_nCtrlChannel = cSettings->m_nCtrlChannel;
    m_nDspChannel = cSettings->m_nDspChannel;
    m_nOverloadBit = cSettings->m_nOverloadBit;
    m_bAvail = cSettings->avail;
    m_nMMode = 0; // SenseChannel::modeAC the default
}

SenseChannelCommon::~SenseChannelCommon()
{
    for(auto range : qAsConst(m_RangeList)) {
        delete range;
    }
}

void SenseChannelCommon::setRangeList(QList<SenseRangeCommon *> &list)
{
    m_RangeList = list;
    setNotifierSenseChannelRangeCat();
    setNotifierSenseChannelRange();
}

QList<SenseRangeCommon *> &SenseChannelCommon::getRangeList()
{
    return m_RangeList;
}

SenseRangeCommon *SenseChannelCommon::getRange(QString &name)
{
    for(auto range : qAsConst(m_RangeList)) {
        if(range->getName() == name)
            return range;
    }
    return nullptr;
}

void SenseChannelCommon::addRangeList(QList<SenseRangeCommon *> &list)
{
    for(auto range : list) {
        m_RangeList.append(range);
    }
    setNotifierSenseChannelRangeCat();
}

void SenseChannelCommon::removeRangeList(QList<SenseRangeCommon *> &list)
{
    for(auto range : list) {
        m_RangeList.removeOne(range);
    }
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

QString SenseChannelCommon::getDescription()
{
    return m_sDescription;
}

quint8 SenseChannelCommon::getCtrlChannel()
{
    return m_nCtrlChannel;
}

void SenseChannelCommon::setDescription(const QString &s)
{
    m_sDescription = s;
}

void SenseChannelCommon::setUnit(QString &s)
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
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"ALIAS", SCPI::isQuery, m_pSCPIInterface, SenseChannel::cmdAlias);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"TYPE", SCPI::isQuery, m_pSCPIInterface, SenseChannel::cmdType);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"UNIT", SCPI::isQuery, m_pSCPIInterface, SenseChannel::cmdUnit);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"DSPCHANNEL", SCPI::isQuery, m_pSCPIInterface, SenseChannel::cmdDspChannel);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"STATUS", SCPI::isQuery, m_pSCPIInterface, SenseChannel::cmdStatus);
    addDelegate(QString("%1%2:STATUS").arg(leadingNodes).arg(m_sName),"RESET", SCPI::isCmd, m_pSCPIInterface, SenseChannel::cmdStatusReset);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"RANGE", SCPI::isQuery | SCPI::isCmdwP, m_pSCPIInterface, SenseChannel::cmdRange, &notifierSenseChannelRange);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"URVALUE", SCPI::isQuery, m_pSCPIInterface, SenseChannel::cmdUrvalue);
    addDelegate(QString("%1%2:RANGE").arg(leadingNodes).arg(m_sName),"CATALOG", SCPI::isQuery, m_pSCPIInterface, SenseChannel::cmdRangeCat, &notifierSenseChannelRangeCat);
    for(auto range : qAsConst(m_RangeList)) {
        connect(range, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
        range->initSCPIConnection(QString("%1%2").arg(leadingNodes).arg(m_sName));
    }
}

void SenseChannelCommon::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case SenseChannel::cmdAlias:
        protoCmd->m_sOutput = m_ReadAlias(protoCmd->m_sInput);
        break;
    case SenseChannel::cmdType:
        protoCmd->m_sOutput = m_ReadType(protoCmd->m_sInput);
        break;
    case SenseChannel::cmdUnit:
        protoCmd->m_sOutput = m_ReadUnit(protoCmd->m_sInput);
        break;
    case SenseChannel::cmdDspChannel:
        protoCmd->m_sOutput = m_ReadDspChannel(protoCmd->m_sInput);
        break;
    case SenseChannel::cmdStatus:
        protoCmd->m_sOutput = m_ReadChannelStatus(protoCmd->m_sInput);
        break;
    case SenseChannel::cmdStatusReset:
        protoCmd->m_sOutput = m_StatusReset(protoCmd->m_sInput);
        break;
    case SenseChannel::cmdRange:
        protoCmd->m_sOutput = scpiReadWriteRange(protoCmd->m_sInput);
        break;
    case SenseChannel::cmdUrvalue:
        protoCmd->m_sOutput = m_ReadUrvalue(protoCmd->m_sInput);
        break;
    case SenseChannel::cmdRangeCat:
        protoCmd->m_sOutput = m_ReadRangeCatalog(protoCmd->m_sInput);
        break;
    }
    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

void SenseChannelCommon::computeJustData()
{
    for(auto range : qAsConst(m_RangeList))
        range->computeJustData();
}

QString SenseChannelCommon::m_ReadAlias(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return getAlias();
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseChannelCommon::m_ReadType(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return QString("0");
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseChannelCommon::m_ReadUnit(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return m_sUnit;
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseChannelCommon::m_ReadDspChannel(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return QString("%1").arg(m_nDspChannel);
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseChannelCommon::m_ReadChannelStatus(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        quint16 status;
        if (m_ctrlFactory->getCriticalStatusController()->readCriticalStatus(status) == ZeraMControllerIo::cmddone ) {
            quint32 r = ((m_bAvail) ? 0 : 1 << 31);
            if (m_nOverloadBit >= 0) { // perhaps this channel has no overload bit
                if ( (status & (1 << m_nOverloadBit))  > 0) {
                    r |= 1;
                }
            }
            return QString("%1").arg(r);
        }
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseChannelCommon::m_StatusReset(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isCommand(1) && (cmd.getParam(0) == "")) {
        if (m_nOverloadBit >= 0)  {
            if (m_ctrlFactory->getCriticalStatusController()->resetCriticalStatus((quint16)(1 << m_nOverloadBit)) == ZeraMControllerIo::cmddone ) {
                return ZSCPI::scpiAnswer[ZSCPI::ack];
            }
            else {
                return ZSCPI::scpiAnswer[ZSCPI::errexec];
            }
        }
        else {
            return ZSCPI::scpiAnswer[ZSCPI::ack];
        }
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseChannelCommon::m_ReadUrvalue(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        for(auto range : qAsConst(m_RangeList)) {
            if (range->getName() == notifierSenseChannelRange.getString()) {
                return QString("%1").arg(range->getUpperRangevalue());
            }
        }
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseChannelCommon::m_ReadRangeCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return notifierSenseChannelRangeCat.getString();
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

void SenseChannelCommon::setNotifierSenseChannelRangeCat()
{
    int i;
    QString s;
    for (i = 0; i < m_RangeList.count()-1; i++)
        s += (m_RangeList.at(i)->getName() + ";");
    s += m_RangeList.at(i)->getName();
    notifierSenseChannelRangeCat = s;
}
