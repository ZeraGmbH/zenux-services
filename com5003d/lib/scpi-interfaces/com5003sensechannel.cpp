#include "com5003sensechannel.h"
#include "senserangecommon.h"
#include "scpiconnection.h"
#include "zscpi_response_definitions.h"
#include <scpi.h>
#include <scpicommand.h>

Com5003SenseChannel::Com5003SenseChannel(cSCPI *scpiinterface,
                                         QString description,
                                         QString unit,
                                         SenseSystem::cChannelSettings *cSettings,
                                         quint8 nr,
                                         AbstractFactoryI2cCtrlPtr ctrlFactory) :
    SenseChannelCommon(scpiinterface,
                       description,
                       unit,
                       cSettings,
                       nr,
                       ctrlFactory)
{
}

void Com5003SenseChannel::setMMode(int m)
{
    m_nMMode = m;
}

QString Com5003SenseChannel::getAlias()
{
    if (m_nMMode == 0)
        return m_sAlias1;
    else
        return m_sAlias2;
}

void Com5003SenseChannel::initSCPIConnection(QString leadingNodes)
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

void Com5003SenseChannel::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
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
        protoCmd->m_sOutput = m_ReadWriteRange(protoCmd->m_sInput);
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

void Com5003SenseChannel::setRangeList(QList<SenseRangeCommon *> &list)
{
    m_RangeList = list;
    setNotifierSenseChannelRangeCat();
    setNotifierSenseChannelRange();
}

QList<SenseRangeCommon *> &Com5003SenseChannel::getRangeList()
{
    return m_RangeList;
}

SenseRangeCommon *Com5003SenseChannel::getRange(QString &name)
{
    for(auto range : qAsConst(m_RangeList)) {
        if(range->getName() == name)
            return range;
    }
    return nullptr;
}

void Com5003SenseChannel::addRangeList(QList<SenseRangeCommon *> &list)
{
    for(auto range : list) {
        m_RangeList.append(range);
    }
    setNotifierSenseChannelRangeCat();
}

void Com5003SenseChannel::removeRangeList(QList<SenseRangeCommon *> &list)
{
    for(auto range : list) {
        m_RangeList.removeOne(range);
    }
    setNotifierSenseChannelRangeCat();
}

quint8 Com5003SenseChannel::getAdjustmentStatus80Mask()
{
    quint8 adj = 255;
    for(auto range : qAsConst(m_RangeList))
        adj &= range->getAdjustmentStatus80Mask();
    return adj;
}

QString Com5003SenseChannel::getName()
{
    return m_sName;
}

QString Com5003SenseChannel::getDescription()
{
    return m_sDescription;
}

quint8 Com5003SenseChannel::getCtrlChannel()
{
    return m_nCtrlChannel;
}

void Com5003SenseChannel::setDescription(const QString &s)
{
    m_sDescription = s;
}

void Com5003SenseChannel::setUnit(QString &s)
{
    m_sUnit = s;
}

bool Com5003SenseChannel::isAvail()
{
    return m_bAvail;
}

void Com5003SenseChannel::initJustData()
{
    for(auto range : qAsConst(m_RangeList))
        range->initJustData();
}

void Com5003SenseChannel::computeJustData()
{
    for(auto range : qAsConst(m_RangeList))
        range->computeJustData();
}

QString Com5003SenseChannel::m_ReadAlias(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return getAlias();
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString Com5003SenseChannel::m_ReadType(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return QString("0");
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString Com5003SenseChannel::m_ReadUnit(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return m_sUnit;
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString Com5003SenseChannel::m_ReadDspChannel(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return QString("%1").arg(m_nDspChannel);
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString Com5003SenseChannel::m_ReadChannelStatus(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        quint16 status;
        if (m_ctrlFactory->getCriticalStatusController()->readCriticalStatus(status) == ZeraMControllerIo::cmddone ) {
            quint32 r;
            r = ((m_bAvail) ? 0 : 1 << 31);
            if ( (status & (1 << m_nOverloadBit))  > 0)
                r |= 1;
            return QString("%1").arg(r);
        }
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString Com5003SenseChannel::m_StatusReset(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isCommand(1) && (cmd.getParam(0) == ""))
    {
        if (m_ctrlFactory->getCriticalStatusController()->resetCriticalStatus((quint16)(1 << m_nOverloadBit)) == ZeraMControllerIo::cmddone )
            return ZSCPI::scpiAnswer[ZSCPI::ack];
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }

    return ZSCPI::scpiAnswer[ZSCPI::nak];
}


void Com5003SenseChannel::setNotifierSenseChannelRange()
{
    quint8 mode, range;

    if (m_ctrlFactory->getMModeController()->readMeasMode(mode) == ZeraMControllerIo::cmddone )
    {
        if (mode == SenseChannel::modeAC) // wir sind im normalberieb
        {
            if (m_ctrlFactory->getRangesController()->readRange(m_nCtrlChannel, range) == ZeraMControllerIo::cmddone )
            {
                int i;
                for (i = 0; i < m_RangeList.count(); i++)
                    if (m_RangeList.at(i)->getSelCode() == range)
                        break;
                notifierSenseChannelRange = m_RangeList.at(i)->getName();
            }
        }
        else
        {
            if (mode == 1)
                notifierSenseChannelRange = "R0V";
            else
                notifierSenseChannelRange = "R10V";
        }
    }
    else
        notifierSenseChannelRange = m_RangeList.at(0)->getName();
}


QString Com5003SenseChannel::m_ReadWriteRange(QString &sInput)
{
    int i;
    quint8 mode;
    cSCPICommand cmd = sInput;

    if (m_ctrlFactory->getMModeController()->readMeasMode(mode) == ZeraMControllerIo::cmddone )
    {
        if (cmd.isQuery())
        {
            return notifierSenseChannelRange.getString();
        }

        else
        {
            if (cmd.isCommand(1))
            {
                QString rng = cmd.getParam(0);
                int anz = m_RangeList.count();
                for  (i = 0; i < anz; i++)
                    if (m_RangeList.at(i)->getName() == rng)
                        break;
                if ( (i < anz) && (m_RangeList.at(i)->getAvail()) )
                {
                    // we know this range and it's available
                    if (m_nMMode == SenseChannel::modeAC)
                    {
                        if (m_ctrlFactory->getRangesController()->setRange(m_nCtrlChannel, m_RangeList.at(i)->getSelCode()) == ZeraMControllerIo::cmddone)
                        {
                            notifierSenseChannelRange = rng;
                            return ZSCPI::scpiAnswer[ZSCPI::ack];
                        }
                        else
                            return ZSCPI::scpiAnswer[ZSCPI::errexec];
                    }
                    else
                    {
                        if (m_RangeList.at(i)->getName() == "R0V")
                        {
                            notifierSenseChannelRange = "R0V";
                            m_ctrlFactory->getMModeController()->setMeasMode(1);
                        }
                        else
                        {
                            notifierSenseChannelRange = "R10V";
                            m_ctrlFactory->getMModeController()->setMeasMode(2);
                        }

                        return ZSCPI::scpiAnswer[ZSCPI::ack];
                    }

                }

            }

            return ZSCPI::scpiAnswer[ZSCPI::nak];
        }
    }

    else
        return ZSCPI::scpiAnswer[ZSCPI::errexec];

}


QString Com5003SenseChannel::m_ReadUrvalue(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        int i;
        for (i = 0; i < m_RangeList.count(); i++)
            if (m_RangeList.at(i)->getName() == notifierSenseChannelRange.getString())
                break;
        return QString("%1").arg(m_RangeList.at(i)->getUpperRangevalue());
    }

    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString Com5003SenseChannel::m_ReadRangeCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        return notifierSenseChannelRangeCat.getString();
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


void Com5003SenseChannel::setNotifierSenseChannelRangeCat()
{
    int i;
    QString s;
    for (i = 0; i < m_RangeList.count()-1; i++)
        s += (m_RangeList.at(i)->getName() + ";");
    s += m_RangeList.at(i)->getName();

    notifierSenseChannelRangeCat = s; // phs. or virt.
}
