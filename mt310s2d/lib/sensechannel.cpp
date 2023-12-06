#include "senserange.h"
#include "scpiconnection.h"
#include "sensechannel.h"
#include "zscpi_response_definitions.h"
#include <scpi.h>
#include <scpicommand.h>
#include "micro-controller-io/atmel.h"

cSenseChannel::cSenseChannel(cSCPI* scpiinterface, QString description, QString unit, SenseSystem::cChannelSettings *cSettings, quint8 nr) :
    ScpiConnection(scpiinterface),
    m_sDescription(description),
    m_sUnit(unit)
{
    m_sName = QString("m%1").arg(nr);
    m_sAlias = cSettings->m_sAlias1;
    m_nCtrlChannel = cSettings->m_nCtrlChannel;
    m_nDspChannel = cSettings->m_nDspChannel;
    m_nOverloadBit = cSettings->m_nOverloadBit;
    m_bAvail = cSettings->avail;
}

cSenseChannel::~cSenseChannel()
{
    for(auto range : qAsConst(m_RangeList)) {
        delete range;
    }
}

void cSenseChannel::initSCPIConnection(QString leadingNodes)
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

void cSenseChannel::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
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
    if (protoCmd->m_bwithOutput) {
        emit cmdExecutionDone(protoCmd);
    }
}

void cSenseChannel::setRangeList(QList<cSenseRange*> &list)
{
    m_RangeList = list;
    setNotifierSenseChannelRangeCat();
    setNotifierSenseChannelRange();
}

QList<cSenseRange *> &cSenseChannel::getRangeList()
{
    return m_RangeList;
}

void cSenseChannel::addRangeList(QList<cSenseRange *> &list)
{
    for(auto range : list) {
        m_RangeList.append(range);
    }
    setNotifierSenseChannelRangeCat();
}

void cSenseChannel::removeRangeList(QList<cSenseRange *> &list)
{
    for(auto range : list) {
        m_RangeList.removeOne(range);
    }
    setNotifierSenseChannelRangeCat();
}

cSenseRange *cSenseChannel::getRange(QString &name)
{
    for(auto range : qAsConst(m_RangeList)) {
        if(range->getName() == name) {
            return range;
        }
    }
    return nullptr;
}

quint8 cSenseChannel::getAdjustmentStatus()
{
    quint8 adj = 255;
    for(auto range : qAsConst(m_RangeList))
        adj &= range->getAdjustmentStatus();
    return adj;
}

QString &cSenseChannel::getName()
{
    return m_sName;
}

QString &cSenseChannel::getAlias()
{
    return m_sAlias;
}

QString &cSenseChannel::getDescription()
{
    return m_sDescription;
}

quint8 cSenseChannel::getCtrlChannel()
{
    return m_nCtrlChannel;
}

void cSenseChannel::setDescription(const QString &s)
{
    m_sDescription = s;
}

void cSenseChannel::setUnit(QString &s)
{
    m_sUnit = s;
}

void cSenseChannel::setMMode(int m)
{
    m_nMMode = m;
    for(auto range : qAsConst(m_RangeList)) {
        range->setMMode(m);
    }
    notifierSenseChannelRangeCat.forceTrigger(); // better we would ask for changed avail ranges and then trigger !!!
    // but we can do this later
}

bool cSenseChannel::isAvail()
{
    return m_bAvail;
}

void cSenseChannel::initJustData()
{
    for(auto range : qAsConst(m_RangeList)) {
        range->initJustData();
    }
}

void cSenseChannel::computeJustData()
{
    for(auto range : qAsConst(m_RangeList)) {
        range->computeJustData();
    }
}

QString cSenseChannel::m_ReadAlias(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        return getAlias();
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString cSenseChannel::m_ReadType(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        return QString("0");
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString cSenseChannel::m_ReadUnit(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        return m_sUnit;
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString cSenseChannel::m_ReadDspChannel(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        return QString("%1").arg(m_nDspChannel);
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString cSenseChannel::m_ReadChannelStatus(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        quint16 status;
        if ( Atmel::getInstance().readCriticalStatus(status) == ZeraMControllerIo::cmddone ) {
            quint32 r;
            r = ((m_bAvail) ? 0 : 1 << 31);
            if (m_nOverloadBit >= 0) { // perhaps this channel has no overload bit
                if ( (status & (1 << m_nOverloadBit))  > 0) {
                    r |= 1;
                }
            }
            return QString("%1").arg(r);
        }
        else {
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
        }
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString cSenseChannel::m_StatusReset(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isCommand(1) && (cmd.getParam(0) == "")) {
        if (m_nOverloadBit >= 0)  {
            if ( Atmel::getInstance().resetCriticalStatus((quint16)(1 << m_nOverloadBit)) == ZeraMControllerIo::cmddone ) {
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

void cSenseChannel::setNotifierSenseChannelRange()
{
    quint8 rSelCode;
    if ( Atmel::getInstance().readRange(m_nCtrlChannel, rSelCode) == ZeraMControllerIo::cmddone ) {
        for(auto range : qAsConst(m_RangeList)) {
            if ( (range->getSelCode() == rSelCode) && (range->isAvail())) {
                notifierSenseChannelRange = range->getName();
                break;
            }
        }
    }
}

QString cSenseChannel::m_ReadWriteRange(QString &sInput)
{
    cSCPICommand cmd = sInput;
    quint8 mode;
    if ( Atmel::getInstance().readMeasMode(mode) == ZeraMControllerIo::cmddone ) {
        if (cmd.isQuery()) {
            return notifierSenseChannelRange.getString();
        }
        else {
            if (cmd.isCommand(1)) {
                QString rng = cmd.getParam(0);
                int anz = m_RangeList.count();
                int i;
                for  (i = 0; i < anz; i++) {
                    if (m_RangeList.at(i)->getName() == rng) {
                        break;
                    }
                }
                if ( (i < anz) && (m_RangeList.at(i)->isAvail()) ) {
                    // we know this range and it's available
                    if ( Atmel::getInstance().setRange(m_nCtrlChannel, m_RangeList.at(i)->getSelCode()) == ZeraMControllerIo::cmddone) {
                        notifierSenseChannelRange = rng;
                        return ZSCPI::scpiAnswer[ZSCPI::ack];
                    }
                    else {
                        return ZSCPI::scpiAnswer[ZSCPI::errexec];
                    }
                }
            }
            return ZSCPI::scpiAnswer[ZSCPI::nak];
        }
    }
    return ZSCPI::scpiAnswer[ZSCPI::errexec];
}

QString cSenseChannel::m_ReadUrvalue(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        for(auto range : qAsConst(m_RangeList)) {
            if (range->getName() == notifierSenseChannelRange.getString()) {
                return QString("%1").arg(range->getUrvalue());
            }
        }
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString cSenseChannel::m_ReadRangeCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        return notifierSenseChannelRangeCat.getString();
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

void cSenseChannel::setNotifierSenseChannelRangeCat()
{
    int i;
    QString s;
    for (i = 0; i < m_RangeList.count()-1; i++) {
        s += (m_RangeList.at(i)->getName() + ";");
    }
    s += m_RangeList.at(i)->getName();
    notifierSenseChannelRangeCat = s;
}