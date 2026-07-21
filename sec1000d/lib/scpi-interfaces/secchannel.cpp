#include "secchannel.h"
#include "zscpi_response_definitions.h"
#include "scpiconnection.h"
#include "secinputsettings.h"
#include "protonetcommand.h"
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

static constexpr const char* baseChnName = "ec";
static constexpr int en_n = 8;
static constexpr int en_p = 9;
static constexpr int direction = 15;
static constexpr int bitSingle = 16;
static constexpr int sssto = 17;
static constexpr int ssarm = 19;
static constexpr int imp_en = 21;
static constexpr int impout_en = 22;
static constexpr int cnt2carry = 23;

SecChannel::SecChannel(std::shared_ptr<cSCPI> scpiInterface,
                       SecCalculatorSettings* esettings,
                       SecInputSettings *inpsettings,
                       quint16 nr,
                       std::function<void (int)> funcSigHandler,
                       AbstractFactoryDeviceNodeSecPtr zdspSupportFactory) :
    ScpiConnection(scpiInterface),
    m_zdspSupportFactory(zdspSupportFactory),
    m_pecalcsettings(esettings),
    m_pInputSettings(inpsettings),
    m_nNr(nr),
    m_funcSigHandler(funcSigHandler)
{
    m_nMyAdress = m_pecalcsettings->getBaseAdress() + (nr << 6);
    m_sName = QString("%1%2").arg(baseChnName).arg(nr);
    m_bSet = false;

    // mrate counter for error measurement or energy comparison in single mode
    CMDIDList.append((1<<en_n) + (1<<bitSingle) + (1<<cnt2carry));
    // mrate counter for error measurement or energy comparison in continous mode
    CMDIDList.append((1<<en_n) + (1<<cnt2carry));
    // vi counter for error measurement or energy comparison
    CMDIDList.append((1<<en_n) + (1<<direction) + (1<<bitSingle) + (3<<sssto) + (2<<ssarm));
}

enum Commands
{
    cmdRegister,
    cmdSetSync,
    cmdSetMux,
    cmdSetCmdid,
    cmdStart,
    cmdStop,
    intAcknowledge
};

void SecChannel::initSCPIConnection(const QString &leadingNodes)
{
    const QString adjLeadNodes = appendTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1%2").arg(adjLeadNodes, getName()), QString("R%1").arg(ECALCREG::CMD), SCPI::isCmdwP | SCPI::isQuery, m_scpiInterface, cmdRegister);
    addDelegate(QString("%1%2").arg(adjLeadNodes, getName()), QString("R%1").arg(ECALCREG::CONF), SCPI::isCmdwP | SCPI::isQuery, m_scpiInterface, cmdRegister);
    addDelegate(QString("%1%2").arg(adjLeadNodes, getName()), QString("R%1").arg(ECALCREG::STATUS), SCPI::isQuery, m_scpiInterface, cmdRegister);
    addDelegate(QString("%1%2").arg(adjLeadNodes, getName()), QString("R%1").arg(ECALCREG::INTMASK), SCPI::isCmdwP | SCPI::isQuery, m_scpiInterface, cmdRegister);
    addDelegate(QString("%1%2").arg(adjLeadNodes, getName()), QString("R%1").arg(ECALCREG::INTREG), SCPI::isCmdwP | SCPI::isQuery, m_scpiInterface, cmdRegister);
    addDelegate(QString("%1%2").arg(adjLeadNodes, getName()), QString("R%1").arg(ECALCREG::MTCNTin), SCPI::isCmdwP | SCPI::isQuery, m_scpiInterface, cmdRegister);
    addDelegate(QString("%1%2").arg(adjLeadNodes, getName()), QString("R%1").arg(ECALCREG::MTCNTfin), SCPI::isQuery, m_scpiInterface, cmdRegister);
    addDelegate(QString("%1%2").arg(adjLeadNodes, getName()), QString("R%1").arg(ECALCREG::MTCNTact), SCPI::isQuery, m_scpiInterface, cmdRegister);
    addDelegate(QString("%1%2").arg(adjLeadNodes, getName()), QString("R%1").arg(ECALCREG::MTPULSin), SCPI::isCmdwP | SCPI::isQuery, m_scpiInterface, cmdRegister);
    addDelegate(QString("%1%2").arg(adjLeadNodes, getName()), QString("R%1").arg(ECALCREG::MTPAUSEin), SCPI::isCmdwP | SCPI::isQuery, m_scpiInterface, cmdRegister);
    addDelegate(QString("%1%2").arg(adjLeadNodes, getName()), QString("R%1").arg(ECALCREG::MTPULS), SCPI::isCmdwP | SCPI::isQuery, m_scpiInterface, cmdRegister);
    addDelegate(QString("%1%2").arg(adjLeadNodes, getName()), QString("R%1").arg(ECALCREG::MTPAUSE), SCPI::isCmdwP | SCPI::isQuery, m_scpiInterface, cmdRegister);
    addDelegate(QString("%1%2").arg(adjLeadNodes, getName()),"SYNC", SCPI::isCmdwP, m_scpiInterface, cmdSetSync);
    addDelegate(QString("%1%2").arg(adjLeadNodes, getName()),"MUX", SCPI::isCmdwP, m_scpiInterface, cmdSetMux);
    addDelegate(QString("%1%2").arg(adjLeadNodes, getName()),"CMDID", SCPI::isCmdwP, m_scpiInterface, cmdSetCmdid);
    addDelegate(QString("%1%2").arg(adjLeadNodes, getName()),"START", SCPI::isCmdwP, m_scpiInterface, cmdStart);
    addDelegate(QString("%1%2").arg(adjLeadNodes, getName()),"STOP", SCPI::isCmdwP, m_scpiInterface, cmdStop);
    addDelegate(QString("%1%2").arg(adjLeadNodes, getName()),"INTACK", SCPI::isCmdwP, m_scpiInterface, intAcknowledge);
}

void SecChannel::executeProtoScpi(int cmdCode, const ProtonetCommandPtr &protoCmd)
{
    switch (cmdCode)
    {
    case cmdRegister:
        readWriteRegister(protoCmd);
        break;
    case cmdSetSync:
        setSync(protoCmd);
        break;
    case cmdSetMux:
        setMux(protoCmd);
        break;
    case cmdSetCmdid:
        setCmdId(protoCmd);
        break;
    case cmdStart:
        start(protoCmd);
        break;
    case cmdStop:
        stop(protoCmd);
        break;
    case intAcknowledge:
        resetInt(protoCmd);
        break;
    }

    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);

}

QString SecChannel::getName() const
{
    return m_sName;
}

bool SecChannel::isfree() const
{
    return !m_bSet;
}

bool SecChannel::set(const QByteArray &id)
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

void SecChannel::readWriteRegister(const ProtonetCommandPtr &protoCmd)
{
    bool ok;
    quint32 reg;

    cSCPICommand cmd = protoCmd->m_sInput;
    QString cmdStr = cmd.getCommandStr();
    QString par = cmdStr.section(':',2,2);
    par.remove(QChar('r'), Qt::CaseInsensitive);
    par.remove(QChar('?'));
    quint8 regInd = par.toInt(&ok);

    AbstractDeviceNodeSecPtr deviceNode = m_zdspSupportFactory->getSecDeviceNode();
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
                deviceNode->lseek(m_nMyAdress + (regInd << 2));
                deviceNode->read((char*) &reg, 4);
                protoCmd->m_sOutput =  QString("%1").arg(reg);
                break;
        }
    }
    else if ( ((protoCmd->m_nSCPIType & SCPI::isCmdwP) !=0) && cmd.isCommand(1) ) {
        if (protoCmd->m_clientId == m_ClientId) { // authorized ?
            par = cmd.getParam(0);
            reg = par.toULong(&ok);
            if (ok)
            {
                deviceNode->lseek(m_nMyAdress + (regInd << 2));
                deviceNode->write((char*) &reg, 4);
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

void SecChannel::setSync(const ProtonetCommandPtr &protoCmd)
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
                    AbstractDeviceNodeSecPtr deviceNode = m_zdspSupportFactory->getSecDeviceNode();
                    deviceNode->lseek(m_nMyAdress + (ECALCREG::CONF << 2));
                    deviceNode->read((char*) &reg, 4);
                    reg = (reg & 0xFFFFFF00) | (chnIndex+1);
                    deviceNode->write((char*) &reg, 4);
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

void SecChannel::setMux(const ProtonetCommandPtr &protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    if (cmd.isCommand(1)) {
        if (protoCmd->m_clientId == m_ClientId) { // authorized ?
            QString par = cmd.getParam(0);
            protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::errval]; // preset
            if (m_pInputSettings->hasInput(par)) {
                quint32 reg;
                AbstractDeviceNodeSecPtr deviceNode = m_zdspSupportFactory->getSecDeviceNode();
                deviceNode->lseek(m_nMyAdress + (ECALCREG::CONF << 2));
                deviceNode->read((char*) &reg, 4);
                reg = (reg & 0xFFFF83FF) | (m_pInputSettings->mux(par) << 10);
                deviceNode->write((char*) &reg, 4);
                protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::ack];
            }
        }
        else
           protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::erraut];
    }
    else
        protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
}

void SecChannel::setCmdId(const ProtonetCommandPtr &protoCmd)
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
                AbstractDeviceNodeSecPtr deviceNode = m_zdspSupportFactory->getSecDeviceNode();
                deviceNode->lseek(m_nMyAdress + (ECALCREG::CONF << 2));
                deviceNode->read((char*) &reg, 4);
                reg = (reg & 0x00007CFF) | CMDIDList.at(cmdId);
                deviceNode->write((char*) &reg, 4);
                protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::ack];
            }
        }
        else
           protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::erraut];
    }
    else
        protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
}

void SecChannel::start(const ProtonetCommandPtr &protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    if (cmd.isCommand(0)) {
        if (protoCmd->m_clientId == m_ClientId) { // authorized ?
            quint32 reg;
            AbstractDeviceNodeSecPtr deviceNode = m_zdspSupportFactory->getSecDeviceNode();
            deviceNode->lseek(m_nMyAdress + (ECALCREG::CMD << 2));
            //SecDeviceNodeSingleton::getInstance()->read((char*) &reg, 4);
            //reg = (reg & 0xFFFFFF3F) | 0x80;
            reg = 0x80;
            deviceNode->write((char*) &reg, 4);
            protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::ack];
        }
        else
           protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::erraut];
    }
    else
        protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
}


void SecChannel::stop(const ProtonetCommandPtr &protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    if (cmd.isCommand(0)) {
        if (protoCmd->m_clientId == m_ClientId) { // authorized ?
            stopErrorCalculator();
            protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::ack];
        }
        else
           protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::erraut];
    }
    else
        protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
}

void SecChannel::resetInt(const ProtonetCommandPtr &protoCmd)
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

void SecChannel::stopErrorCalculator()
{
    quint32 reg;
    AbstractDeviceNodeSecPtr deviceNode = m_zdspSupportFactory->getSecDeviceNode();
    deviceNode->lseek(m_nMyAdress + (ECALCREG::CMD << 2));
    //read(m_devFileDescriptor,(char*) &reg, 4);
    //reg = (reg & 0xFFFFFF3F) | 0x40;
    reg = 0x40;
    deviceNode->write((char*) &reg, 4);
    // we must reset the stop bit in command register because ecalculator will never start
    reg = 0x0;
    deviceNode->write((char*) &reg, 4);
}


void SecChannel::resetInterrupt(quint8 interrupt)
{
    quint32 reg;

    notifierECalcChannelIntReg.clrValue(interrupt);
    AbstractDeviceNodeSecPtr deviceNode = m_zdspSupportFactory->getSecDeviceNode();
    deviceNode->lseek(m_nMyAdress + (ECALCREG::INTREG << 2));
    deviceNode->read((char*) &reg, 4);
    reg = (reg & 0xF) & ~interrupt;
    deviceNode->write((char*) &reg, 4);
}
