#include <unistd.h>
#include <fcntl.h>

#include <QList>
#include <QString>

#include <scpi.h>
#include <scpicommand.h>

#include "sec1000d.h"
#include <scpiconnection.h>
#include "ecalcchannel.h"
#include "fpgasettings.h"
#include "inputsettings.h"
#include "protonetcommand.h"

extern void SigHandler(int);

cECalculatorChannel::cECalculatorChannel(cSEC1000dServer* server, cECalculatorSettings* esettings, cFPGASettings* fsettings, cInputSettings *inpsettings, quint16 nr) :
    cSCPIConnection(ScpiSingletonFactory::getScpiObj(ServerName)),
    m_pMyServer(server),
    m_pecalcsettings(esettings),
    m_pfpgasettings(fsettings),
    m_pInputSettings(inpsettings),
    m_nNr(nr)
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


cECalculatorChannel::~cECalculatorChannel()
{
}


void cECalculatorChannel::initSCPIConnection(QString leadingNodes)
{
    cSCPIDelegate* delegate;

    if (leadingNodes != "")
        leadingNodes += ":";

    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName), QString("R%1").arg(ECALCREG::CMD), SCPI::isCmdwP | SCPI::isQuery, m_pSCPIInterface, ECalcChannel::cmdRegister);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName), QString("R%1").arg(ECALCREG::CONF), SCPI::isCmdwP | SCPI::isQuery, m_pSCPIInterface, ECalcChannel::cmdRegister);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName), QString("R%1").arg(ECALCREG::STATUS), SCPI::isQuery, m_pSCPIInterface, ECalcChannel::cmdRegister);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName), QString("R%1").arg(ECALCREG::INTMASK), SCPI::isCmdwP | SCPI::isQuery, m_pSCPIInterface, ECalcChannel::cmdRegister);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName), QString("R%1").arg(ECALCREG::INTREG), SCPI::isCmdwP | SCPI::isQuery, m_pSCPIInterface, ECalcChannel::cmdRegister);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName), QString("R%1").arg(ECALCREG::MTCNTin), SCPI::isCmdwP | SCPI::isQuery, m_pSCPIInterface, ECalcChannel::cmdRegister);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName), QString("R%1").arg(ECALCREG::MTCNTfin), SCPI::isQuery, m_pSCPIInterface, ECalcChannel::cmdRegister);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName), QString("R%1").arg(ECALCREG::MTCNTact), SCPI::isQuery, m_pSCPIInterface, ECalcChannel::cmdRegister);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName), QString("R%1").arg(ECALCREG::MTPULSin), SCPI::isCmdwP | SCPI::isQuery, m_pSCPIInterface, ECalcChannel::cmdRegister);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName), QString("R%1").arg(ECALCREG::MTPAUSEin), SCPI::isCmdwP | SCPI::isQuery, m_pSCPIInterface, ECalcChannel::cmdRegister);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName), QString("R%1").arg(ECALCREG::MTPULS), SCPI::isCmdwP | SCPI::isQuery, m_pSCPIInterface, ECalcChannel::cmdRegister);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName), QString("R%1").arg(ECALCREG::MTPAUSE), SCPI::isCmdwP | SCPI::isQuery, m_pSCPIInterface, ECalcChannel::cmdRegister);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"SYNC", SCPI::isCmdwP, m_pSCPIInterface, ECalcChannel::setSync);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"MUX", SCPI::isCmdwP, m_pSCPIInterface, ECalcChannel::setMux);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"CMDID", SCPI::isCmdwP, m_pSCPIInterface, ECalcChannel::setCmdid);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"START", SCPI::isCmdwP, m_pSCPIInterface, ECalcChannel::start);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"STOP", SCPI::isCmdwP, m_pSCPIInterface, ECalcChannel::stop);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"INTACK", SCPI::isCmdwP, m_pSCPIInterface, ECalcChannel::intAcknowledge);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
}


void cECalculatorChannel::executeCommand(int cmdCode, cProtonetCommand *protoCmd)
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


QString &cECalculatorChannel::getName()
{
    return m_sName;
}


bool cECalculatorChannel::isfree()
{
    return !m_bSet;
}


bool cECalculatorChannel::set(QByteArray id)
{
    bool ret = !m_bSet;
    if (ret)
        m_ClientId = id;
    m_bSet = true;
    return ret;
}


void cECalculatorChannel::free()
{
    m_bSet = false;
}


void cECalculatorChannel::setIntReg(quint8 reg)
{
    notifierECalcChannelIntReg.setValue(reg);
}


void cECalculatorChannel::m_ReadWriteRegister(cProtonetCommand *protoCmd)
{
    QString par, cmdStr;
    bool ok;
    quint8 regInd;
    quint32 reg;

    cSCPICommand cmd = protoCmd->m_sInput;
    cmdStr = cmd.getCommandStr();
    par = cmdStr.section(':',2,2);
    par.remove(QChar('r'), Qt::CaseInsensitive);
    par.remove(QChar('?'));
    regInd = par.toInt(&ok);

    if ( ((protoCmd->m_nSCPIType & SCPI::isQuery) != 0) && cmd.isQuery() )
    {
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
            lseek(m_pMyServer->DevFileDescriptor, m_nMyAdress + (regInd << 2), 0);
            read(m_pMyServer->DevFileDescriptor,(char*) &reg, 4);
            protoCmd->m_sOutput =  QString("%1").arg(reg);
        }
    }

    else

        if ( ((protoCmd->m_nSCPIType & SCPI::isCmdwP) !=0) && cmd.isCommand(1) )
        {
            if (protoCmd->m_clientId == m_ClientId) // authorized ?
            {
                par = cmd.getParam(0);
                reg = par.toULong(&ok);
                if (ok)
                {
                    lseek(m_pMyServer->DevFileDescriptor, m_nMyAdress + (regInd << 2), 0);
                    write(m_pMyServer->DevFileDescriptor,(char*) &reg, 4);
                    protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::ack];
                }
                else
                    protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];
            }
            else
                protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::erraut];
        }

        else

            protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];
}


void cECalculatorChannel::m_setSync(cProtonetCommand *protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    if (cmd.isCommand(1))
    {
        if (protoCmd->m_clientId == m_ClientId) // authorized ?
        {
            QString par;
            par = cmd.getParam(0);
            protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::errval]; // preset
            if (par.contains("ec"))
            {
                bool ok;
                quint32 chnIndex;
                par.remove(baseChnName);
                chnIndex = par.toULong(&ok);
                if (ok && (chnIndex <= m_pecalcsettings->getNumber()) )
                {
                    quint32 reg;
                    lseek(m_pMyServer->DevFileDescriptor, m_nMyAdress + (ECALCREG::CONF << 2), 0);
                    read(m_pMyServer->DevFileDescriptor,(char*) &reg, 4);
                    reg = (reg & 0xFFFFFF00) | (chnIndex+1);
                    write(m_pMyServer->DevFileDescriptor,(char*) &reg, 4);
                    protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::ack];
                }
            }
        }
        else
           protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::erraut];
    }
    else
        protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];
}


void cECalculatorChannel::m_setMux(cProtonetCommand *protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    if (cmd.isCommand(1))
    {
        if (protoCmd->m_clientId == m_ClientId) // authorized ?
        {
            QString par;
            par = cmd.getParam(0);
            protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::errval]; // preset

            if (m_pInputSettings->hasInput(par))
            {
                quint32 reg;
                lseek(m_pMyServer->DevFileDescriptor, m_nMyAdress + (ECALCREG::CONF << 2), 0);
                read(m_pMyServer->DevFileDescriptor,(char*) &reg, 4);
                reg = (reg & 0xFFFF83FF) | (m_pInputSettings->mux(par) << 10);
                write(m_pMyServer->DevFileDescriptor,(char*) &reg, 4);
                protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::ack];
            }
        }
        else
           protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::erraut];
    }
    else
        protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];
}


void cECalculatorChannel::m_setCmdId(cProtonetCommand *protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    if (cmd.isCommand(1))
    {
        if (protoCmd->m_clientId == m_ClientId) // authorized ?
        {
            bool ok;
            quint32 cmdId;
            QString par;
            par = cmd.getParam(0);
            protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::errval]; // preset
            cmdId = par.toULong(&ok);
            if (ok && (cmdId < 3) )
            {
                quint32 reg;
                lseek(m_pMyServer->DevFileDescriptor, m_nMyAdress + (ECALCREG::CONF << 2), 0);
                read(m_pMyServer->DevFileDescriptor,(char*) &reg, 4);
                reg = (reg & 0x00007CFF) | CMDIDList.at(cmdId);
                write(m_pMyServer->DevFileDescriptor,(char*) &reg, 4);
                protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::ack];
            }
        }
        else
           protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::erraut];
    }
    else
        protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];
}


void cECalculatorChannel::m_start(cProtonetCommand *protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    if (cmd.isCommand(0))
    {
        if (protoCmd->m_clientId == m_ClientId) // authorized ?
        {
            quint32 reg;
            lseek(m_pMyServer->DevFileDescriptor, m_nMyAdress + (ECALCREG::CMD << 2), 0);
            //read(m_pMyServer->DevFileDescriptor,(char*) &reg, 4);
            //reg = (reg & 0xFFFFFF3F) | 0x80;
            reg = 0x80;
            write(m_pMyServer->DevFileDescriptor,(char*) &reg, 4);
            protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::ack];
        }
        else
           protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::erraut];
    }
    else
        protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];
}


void cECalculatorChannel::m_stop(cProtonetCommand *protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    if (cmd.isCommand(0))
    {
        if (protoCmd->m_clientId == m_ClientId) // authorized ?
        {
            m_StopErrorCalculator();
            protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::ack];
        }
        else
           protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::erraut];
    }
    else
        protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];
}


void cECalculatorChannel::m_resetInt(cProtonetCommand *protoCmd)
{
    bool ok;
    cSCPICommand cmd = protoCmd->m_sInput;
    QString par;
    quint32 interrupt;

    if (cmd.isCommand(1))
    {
        if (protoCmd->m_clientId == m_ClientId) // authorized ?
        {
            par = cmd.getParam(0);
            interrupt = par.toULong(&ok);
            if (ok)
            {
                m_resetInterrupt(interrupt);
                SigHandler(0); // we do so as if the interrupt handler had seen another edge
            }
            else
                protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];
        }
        else
            protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::erraut];
    }
    else
        protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];

}


void cECalculatorChannel::m_StopErrorCalculator()
{
    quint32 reg;

    lseek(m_pMyServer->DevFileDescriptor, m_nMyAdress + (ECALCREG::CMD << 2), 0);
    //read(m_pMyServer->DevFileDescriptor,(char*) &reg, 4);
    //reg = (reg & 0xFFFFFF3F) | 0x40;
    reg = 0x40;
    write(m_pMyServer->DevFileDescriptor,(char*) &reg, 4);
    // we must reset the stop bit in command register because ecalculator will never start
    reg = 0x0;
    write(m_pMyServer->DevFileDescriptor,(char*) &reg, 4);
}


void cECalculatorChannel::m_resetInterrupt(quint8 interrupt)
{
    quint32 reg;

    notifierECalcChannelIntReg.clrValue(interrupt);
    lseek(m_pMyServer->DevFileDescriptor, m_nMyAdress + (ECALCREG::INTREG << 2), 0);
    read(m_pMyServer->DevFileDescriptor,(char*) &reg, 4);
    reg = (reg & 0xF) & ~interrupt;
    write(m_pMyServer->DevFileDescriptor,(char*) &reg, 4);
}


