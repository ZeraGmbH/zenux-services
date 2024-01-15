#include "com5003sensechannel.h"
#include "sensesettings.h"
#include "micro-controller-io/atmel.h"
#include "zscpi_response_definitions.h"
#include <QList>
#include <QString>
#include <scpi.h>
#include <scpicommand.h>
#include "com5003senserange.h"
#include "scpiconnection.h"

Com5003SenseChannel::Com5003SenseChannel(cSCPI *scpiinterface,
                                         QString description,
                                         QString unit,
                                         SenseSystem::cChannelSettings *cSettings,
                                         quint8 nr,
                                         FactoryControllerAbstractPtr ctrlFactory) :
    ScpiConnection(scpiinterface),
    m_ctrlFactory(ctrlFactory),
    m_sDescription(description),
    m_sUnit(unit)
{
    m_sName = QString("m%1").arg(nr);
    m_sAlias[0] = cSettings->m_sAlias1;
    m_sAlias[1] = cSettings->m_sAlias2;
    m_nCtrlChannel = cSettings->m_nCtrlChannel;
    m_nDspChannel = cSettings->m_nDspChannel;
    m_nOverloadBit = cSettings->m_nOverloadBit;
    m_bAvail = cSettings->avail;
    m_nMMode = SenseChannel::modeAC; // the default
}


Com5003SenseChannel::~Com5003SenseChannel()
{
    for (int i = 0; i < m_RangeList.count(); i++)
        delete m_RangeList.at(i);
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
    for (int i = 0;i < m_RangeList.count(); i++) {
        connect(m_RangeList.at(i), &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
        m_RangeList.at(i)->initSCPIConnection(QString("%1%2").arg(leadingNodes).arg(m_sName));
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


void Com5003SenseChannel::setRangeList(QList<Com5003SenseRange*> &list)
{
    m_RangeList = list;
    setNotifierSenseChannelRangeCat();
    setNotifierSenseChannelRange();
}


QList<Com5003SenseRange *> &Com5003SenseChannel::getRangeList()
{
    return m_RangeList;
}


Com5003SenseRange *Com5003SenseChannel::getRange(QString &name)
{
    int i;
    for (i = 0; i < m_RangeList.count(); i++)
        if (m_RangeList.at(i)->getName() == name)
            break;

    if (i < m_RangeList.count())
        return m_RangeList.at(i);
    else
        return 0;
}


quint8 Com5003SenseChannel::getAdjustmentStatus80Mask()
{
    quint8 adj = 255;
    for(auto range : qAsConst(m_RangeList))
        adj &= range->getAdjustmentStatus80Mask();
    return adj;
}


QString &Com5003SenseChannel::getName()
{
    return m_sName;
}


QString &Com5003SenseChannel::getAlias()
{
    if (m_nMMode == 0)
        return m_sAlias[0];
    else
        return m_sAlias[1];
}


QString &Com5003SenseChannel::getDescription()
{
    return m_sDescription;
}


void Com5003SenseChannel::setDescription(const QString &s)
{
    m_sDescription = s;
}


void Com5003SenseChannel::setUnit(QString &s)
{
    m_sUnit = s;
}


void Com5003SenseChannel::setMMode(int m)
{
    m_nMMode = m;
}


bool Com5003SenseChannel::isAvail()
{
    return m_bAvail;
}


void Com5003SenseChannel::initJustData()
{
    for (int i = 0; i < m_RangeList.count(); i++)
        m_RangeList.at(i)->initJustData();
}


void Com5003SenseChannel::computeJustData()
{
    for (int i = 0; i < m_RangeList.count(); i++)
        m_RangeList.at(i)->computeJustData();
}


QString Com5003SenseChannel::m_ReadAlias(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        if (m_nMMode == 0)
            return m_sAlias[0];
        else
            return m_sAlias[1];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString Com5003SenseChannel::m_ReadType(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return QString("0");
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString Com5003SenseChannel::m_ReadUnit(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return m_sUnit;
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString Com5003SenseChannel::m_ReadDspChannel(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return QString("%1").arg(m_nDspChannel);
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString Com5003SenseChannel::m_ReadChannelStatus(QString &sInput)
{
    quint16 status;
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        if ( Atmel::getInstance().readCriticalStatus(status) == ZeraMControllerIo::cmddone )
        {
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
        if ( Atmel::getInstance().resetCriticalStatus((quint16)(1 << m_nOverloadBit)) == ZeraMControllerIo::cmddone )
            return ZSCPI::scpiAnswer[ZSCPI::ack];
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }

    return ZSCPI::scpiAnswer[ZSCPI::nak];
}


void Com5003SenseChannel::setNotifierSenseChannelRange()
{
    quint8 mode, range;

    if ( Atmel::getInstance().readMeasMode(mode) == ZeraMControllerIo::cmddone )
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

    if ( Atmel::getInstance().readMeasMode(mode) == ZeraMControllerIo::cmddone )
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
                            Atmel::getInstance().setMeasMode(1);
                        }
                        else
                        {
                            notifierSenseChannelRange = "R10V";
                            Atmel::getInstance().setMeasMode(2);
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
