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

void Com5003SenseChannel::setNotifierSenseChannelRange()
{
    quint8 mode, rSelCode;
    if (m_ctrlFactory->getMModeController()->readMeasMode(mode) == ZeraMControllerIo::cmddone ) {
        if (mode == SenseChannel::modeAC) { // wir sind im normalberieb
            if (m_ctrlFactory->getRangesController()->readRange(m_nCtrlChannel, rSelCode) == ZeraMControllerIo::cmddone ) {
                for(auto range : qAsConst(m_RangeList)) {
                    if ( (range->getSelCode() == rSelCode) && (range->getAvail())) {
                        notifierSenseChannelRange = range->getName();
                        break;
                    }
                }
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

QString Com5003SenseChannel::scpiReadWriteRange(QString &sInput)
{
    cSCPICommand cmd = sInput;
    quint8 mode;
    if (m_ctrlFactory->getMModeController()->readMeasMode(mode) == ZeraMControllerIo::cmddone ) {
        if (cmd.isQuery())
            return notifierSenseChannelRange.getString();
        else {
            if (cmd.isCommand(1)) {
                QString rng = cmd.getParam(0);
                int anz = m_RangeList.count();
                int i;
                for  (i = 0; i < anz; i++) {
                    if (m_RangeList.at(i)->getName() == rng)
                        break;
                }
                if ( (i < anz) && (m_RangeList.at(i)->getAvail()) ) {
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
    return ZSCPI::scpiAnswer[ZSCPI::errexec];
}
