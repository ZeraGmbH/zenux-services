#include "zscpi_response_definitions.h"
#include "scpiconnection.h"
#include "secchannel.h"
#include "secinputsettings.h"
#include "protonetcommand.h"
#include "scpisingletonfactory.h"
#include "zscpi_response_definitions.h"
#include <scpi.h>
#include <scpicommand.h>
#include <QList>
#include <QString>
#include <unistd.h>

namespace ECALCREG {
enum { CMD, CONF, STATUS, INTMASK, INTREG, MTCNTin, MTCNTfin, MTCNTact, MTPULSin = 12, MTPAUSEin, MTPULS, MTPAUSE};
}

namespace ECALCCMDID {
enum { COUNTEDGE = 1, COUNTRESET, ERRORMEASMASTER, ERRORMEASSLAVE};
}

SecChannel::SecChannel(int devFileDescriptor, SecCalculatorSettings* esettings, SecInputSettings *inpsettings, quint16 nr, std::function<void (int)> funcSigHandler) :
    ScpiConnection(ScpiSingletonFactory::getScpiObj()),
    m_devFileDescriptor(devFileDescriptor),
    m_pecalcsettings(esettings),
    m_pInputSettings(inpsettings),
    m_nNr(nr),
    m_funcSigHandler(funcSigHandler)
{
    m_nBaseAdress = m_pecalcsettings->getBaseAdress();
    m_nMyAdress = m_nBaseAdress + (nr << 6);
    m_sName = QString("%1%2").arg(baseChnName).arg(nr);
    m_bSet = false;

    // mrate counter for error measurement or energy comparison in single mode
    CMDIDList.append((1<<en_n) + (1<<bitSingle) + (1<<cnt2carry));
    // mrate counter for error measurement or energy comparison in continous mode
    CMDIDList.append((1<<en_n) + (1<<cnt2carry));
    // vi counter for error measurement or energy comparison
    CMDIDList.append((1<<en_n) + (1<<direction) + (1<<bitSingle) + (3<<sssto) + (2<<ssarm));
}


SecChannel::~SecChannel()
{
}


void SecChannel::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName), QString("R%1").arg(ECALCREG::CMD), SCPI::isCmdwP | SCPI::isQuery, m_pSCPIInterface, ECalcChannel::cmdRegister);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName), QString("R%1").arg(ECALCREG::CONF), SCPI::isCmdwP | SCPI::isQuery, m_pSCPIInterface, ECalcChannel::cmdRegister);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName), QString("R%1").arg(ECALCREG::STATUS), SCPI::isQuery, m_pSCPIInterface, ECalcChannel::cmdRegister);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName), QString("R%1").arg(ECALCREG::INTMASK), SCPI::isCmdwP | SCPI::isQuery, m_pSCPIInterface, ECalcChannel::cmdRegister);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName), QString("R%1").arg(ECALCREG::INTREG), SCPI::isCmdwP | SCPI::isQuery, m_pSCPIInterface, ECalcChannel::cmdRegister);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName), QString("R%1").arg(ECALCREG::MTCNTin), SCPI::isCmdwP | SCPI::isQuery, m_pSCPIInterface, ECalcChannel::cmdRegister);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName), QString("R%1").arg(ECALCREG::MTCNTfin), SCPI::isQuery, m_pSCPIInterface, ECalcChannel::cmdRegister);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName), QString("R%1").arg(ECALCREG::MTCNTact), SCPI::isQuery, m_pSCPIInterface, ECalcChannel::cmdRegister);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName), QString("R%1").arg(ECALCREG::MTPULSin), SCPI::isCmdwP | SCPI::isQuery, m_pSCPIInterface, ECalcChannel::cmdRegister);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName), QString("R%1").arg(ECALCREG::MTPAUSEin), SCPI::isCmdwP | SCPI::isQuery, m_pSCPIInterface, ECalcChannel::cmdRegister);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName), QString("R%1").arg(ECALCREG::MTPULS), SCPI::isCmdwP | SCPI::isQuery, m_pSCPIInterface, ECalcChannel::cmdRegister);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName), QString("R%1").arg(ECALCREG::MTPAUSE), SCPI::isCmdwP | SCPI::isQuery, m_pSCPIInterface, ECalcChannel::cmdRegister);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"SYNC", SCPI::isCmdwP, m_pSCPIInterface, ECalcChannel::setSync);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"MUX", SCPI::isCmdwP, m_pSCPIInterface, ECalcChannel::setMux);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"CMDID", SCPI::isCmdwP, m_pSCPIInterface, ECalcChannel::setCmdid);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"START", SCPI::isCmdwP, m_pSCPIInterface, ECalcChannel::start);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"STOP", SCPI::isCmdwP, m_pSCPIInterface, ECalcChannel::stop);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"INTACK", SCPI::isCmdwP, m_pSCPIInterface, ECalcChannel::intAcknowledge);
}


void SecChannel::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case ECalcChannel::cmdRegister:
        m_ReadWriteRegister(protoCmd);
        break;
    case ECalcChannel::setSync:
        m_setSync(protoCmd);
        break;
    case ECalcChannel::setMux:
        m_setMux(protoCmd);
        break;
    case ECalcChannel::setCmdid:
        m_setCmdId(protoCmd);
        break;
    case ECalcChannel::start:
        m_start(protoCmd);
        break;
    case ECalcChannel::stop:
        m_stop(protoCmd);
        break;
    case ECalcChannel::intAcknowledge:
        m_resetInt(protoCmd);
        break;
    }

    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);

}


QString &SecChannel::getName()
{
    return m_sName;
}


bool SecChannel::isfree()
{
    return !m_bSet;
}


bool SecChannel::set(QByteArray id)
{
    bool ret = !m_bSet;
    if (ret)
        m_ClientId = id;
    m_bSet = true;
    return ret;
}


void SecChannel::free()
{
    m_bSet = false;
}


void SecChannel::setIntReg(quint8 reg)
{
    notifierECalcChannelIntReg.setValue(reg);
}


void SecChannel::m_ReadWriteRegister(cProtonetCommand *protoCmd)
{
    bool ok;
    quint32 reg;

    cSCPICommand cmd = protoCmd->m_sInput;
    QString cmdStr = cmd.getCommandStr();
    QString par = cmdStr.section(':',2,2);
    par.remove(QChar('r'), Qt::CaseInsensitive);
    par.remove(QChar('?'));
    quint8 regInd = par.toInt(&ok);

    if ( ((protoCmd->m_nSCPIType & SCPI::isQuery) != 0) && cmd.isQuery() ) {
        switch (regInd)
        {
            case ECALCREG::INTREG:
                emit valNotifier(&notifierECalcChannelIntReg);
                protoCmd->m_sOutput =  QString("%1").arg(notifierECalcChannelIntReg.getValue()); // we only return the notifiers value
                break;

            //case ECALCREG::CMD:
            //case ECALCREG::CONF:
            //case ECALCREG::STATUS:
            //case ECALCREG::INTMASK:
            //case ECALCREG::MTCNTin:
            //case ECALCREG::MTCNTfin:
            //case ECALCREG::MTCNTact:
            //case ECALCREG::MTPULSin:
            //case ECALCREG::MTPAUSEin:
            //case ECALCREG::MTPULS:
            //case ECALCREG::MTPAUSE:

            default:
            lseek(m_devFileDescriptor, m_nMyAdress + (regInd << 2), 0);
            read(m_devFileDescriptor,(char*) &reg, 4);
            protoCmd->m_sOutput =  QString("%1").arg(reg);
        }
    }
    else if ( ((protoCmd->m_nSCPIType & SCPI::isCmdwP) !=0) && cmd.isCommand(1) ) {
        if (protoCmd->m_clientId == m_ClientId) { // authorized ?
            par = cmd.getParam(0);
            reg = par.toULong(&ok);
            if (ok)
            {
                lseek(m_devFileDescriptor, m_nMyAdress + (regInd << 2), 0);
                write(m_devFileDescriptor,(char*) &reg, 4);
                protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::ack];
            }
            else
                protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
        }
        else
            protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::erraut];
    }
    else
        protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
}

void SecChannel::m_setSync(cProtonetCommand *protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    if (cmd.isCommand(1)) {
        if (protoCmd->m_clientId == m_ClientId) { // authorized ?
            QString par = cmd.getParam(0);
            protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::errval]; // preset
            if (par.contains("ec")) {
                par.remove(baseChnName);
                bool ok;
                quint32 chnIndex = par.toULong(&ok);
                if (ok && (chnIndex <= m_pecalcsettings->getNumber()) ) {
                    quint32 reg;
                    lseek(m_devFileDescriptor, m_nMyAdress + (ECALCREG::CONF << 2), 0);
                    read(m_devFileDescriptor,(char*) &reg, 4);
                    reg = (reg & 0xFFFFFF00) | (chnIndex+1);
                    write(m_devFileDescriptor,(char*) &reg, 4);
                    protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::ack];
                }
            }
        }
        else
           protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::erraut];
    }
    else
        protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
}

void SecChannel::m_setMux(cProtonetCommand *protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    if (cmd.isCommand(1)) {
        if (protoCmd->m_clientId == m_ClientId) { // authorized ?
            QString par = cmd.getParam(0);
            protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::errval]; // preset
            if (m_pInputSettings->hasInput(par)) {
                quint32 reg;
                lseek(m_devFileDescriptor, m_nMyAdress + (ECALCREG::CONF << 2), 0);
                read(m_devFileDescriptor,(char*) &reg, 4);
                reg = (reg & 0xFFFF83FF) | (m_pInputSettings->mux(par) << 10);
                write(m_devFileDescriptor,(char*) &reg, 4);
                protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::ack];
            }
        }
        else
           protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::erraut];
    }
    else
        protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
}

void SecChannel::m_setCmdId(cProtonetCommand *protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    if (cmd.isCommand(1)) {
        if (protoCmd->m_clientId == m_ClientId) { // authorized ?
            QString par = cmd.getParam(0);
            protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::errval]; // preset
            bool ok;
            quint32 cmdId = par.toULong(&ok);
            if (ok && (cmdId < 3) ) {
                quint32 reg;
                lseek(m_devFileDescriptor, m_nMyAdress + (ECALCREG::CONF << 2), 0);
                read(m_devFileDescriptor,(char*) &reg, 4);
                reg = (reg & 0x00007CFF) | CMDIDList.at(cmdId);
                write(m_devFileDescriptor,(char*) &reg, 4);
                protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::ack];
            }
        }
        else
           protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::erraut];
    }
    else
        protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
}

void SecChannel::m_start(cProtonetCommand *protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    if (cmd.isCommand(0)) {
        if (protoCmd->m_clientId == m_ClientId) { // authorized ?
            quint32 reg;
            lseek(m_devFileDescriptor, m_nMyAdress + (ECALCREG::CMD << 2), 0);
            //read(m_devFileDescriptor,(char*) &reg, 4);
            //reg = (reg & 0xFFFFFF3F) | 0x80;
            reg = 0x80;
            write(m_devFileDescriptor,(char*) &reg, 4);
            protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::ack];
        }
        else
           protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::erraut];
    }
    else
        protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
}


void SecChannel::m_stop(cProtonetCommand *protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    if (cmd.isCommand(0)) {
        if (protoCmd->m_clientId == m_ClientId) { // authorized ?
            m_StopErrorCalculator();
            protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::ack];
        }
        else
           protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::erraut];
    }
    else
        protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
}

void SecChannel::m_resetInt(cProtonetCommand *protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    if (cmd.isCommand(1)) {
        if (protoCmd->m_clientId == m_ClientId) { // authorized ?
            QString par = cmd.getParam(0);
            bool ok;
            quint32 interrupt = par.toULong(&ok);
            if(ok) {
                resetInterrupt(interrupt);
                m_funcSigHandler(0); // we do so as if the interrupt handler had seen another edge
            }
            else
                protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
        }
        else
            protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::erraut];
    }
    else
        protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];

}

void SecChannel::m_StopErrorCalculator()
{
    quint32 reg;

    lseek(m_devFileDescriptor, m_nMyAdress + (ECALCREG::CMD << 2), 0);
    //read(m_devFileDescriptor,(char*) &reg, 4);
    //reg = (reg & 0xFFFFFF3F) | 0x40;
    reg = 0x40;
    write(m_devFileDescriptor,(char*) &reg, 4);
    // we must reset the stop bit in command register because ecalculator will never start
    reg = 0x0;
    write(m_devFileDescriptor,(char*) &reg, 4);
}


void SecChannel::resetInterrupt(quint8 interrupt)
{
    quint32 reg;

    notifierECalcChannelIntReg.clrValue(interrupt);
    lseek(m_devFileDescriptor, m_nMyAdress + (ECALCREG::INTREG << 2), 0);
    read(m_devFileDescriptor,(char*) &reg, 4);
    reg = (reg & 0xF) & ~interrupt;
    write(m_devFileDescriptor,(char*) &reg, 4);
}


