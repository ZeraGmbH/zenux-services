#include "secchannel.h"
#include "secinterface.h"
#include "zscpi_response_definitions.h"
#include "scpiconnection.h"
#include "secinputsettings.h"
#include "protonetcommand.h"
#include <scpi.h>
#include <scpicommand.h>
#include <QList>
#include <QString>
#include <unistd.h>

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

static const quint32 SecConfigs[] = {
    // SINGLEERRORMASTER: mrate counter for error measurement or energy comparison in single mode
    (1<<en_n) + (1<<bitSingle) + (1<<cnt2carry),
    // CONTERRORMASTER: mrate counter for error measurement or energy comparison in continous mode
    (1<<en_n) + (1<<cnt2carry),
    // ERRORMEASSLAVE: vi counter for error measurement or energy comparison
    (1<<en_n) + (1<<direction) + (1<<bitSingle) + (3<<sssto) + (2<<ssarm),
    // FLANKCOUNT
    (1<<bitSingle) + (1<<direction) + (1<<en_p) + (1<<en_n)
};

static constexpr int SecConfigCount = sizeof(SecConfigs) / sizeof(quint32);

SecChannel::SecChannel(const std::shared_ptr<cSCPI> &scpiInterface,
                       SecCalculatorSettings* esettings,
                       SecInputSettings *inpsettings,
                       quint16 nr,
                       const AbstractFactoryDeviceNodeSecPtr &zdspSupportFactory) :
    ScpiConnection(scpiInterface),
    m_zdspSupportFactory(zdspSupportFactory),
    m_pecalcsettings(esettings),
    m_pInputSettings(inpsettings),
    m_nNr(nr),
    m_nMyAdress(m_pecalcsettings->getBaseAdress() + (nr << 6)),
    m_sName(QString("%1%2").arg(baseChnName).arg(nr))
{
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
    if (m_bSet) {
        // In case:
        // * A module implementor forgets
        // * We do TDD with remote modulemanager where unclean termination during measurement is likely
        // => Be polite and stop measurement so next module using this channel won't find channel in undefined
        //    and possibly non working condition
        stopErrorCalculator();
        m_bSet = false;
    }
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
                deviceNode->read(reinterpret_cast<char*>(&reg), 4);
                protoCmd->m_sOutput =  QString("%1").arg(reg);
                break;
        }
    }
    else if ( ((protoCmd->m_nSCPIType & SCPI::isCmdwP) !=0) && cmd.isCommand(1) ) {
        if (protoCmd->m_clientId == m_ClientId) { // authorized ?
            par = cmd.getParam(0);
            reg = par.toULong(&ok);
            if (ok) {
                deviceNode->lseek(m_nMyAdress + (regInd << 2));
                deviceNode->write(reinterpret_cast<char*>(&reg), 4);
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
            if (par.contains(baseChnName)) {
                par.remove(baseChnName);
                bool ok;
                int chnIndex = par.toULong(&ok);
                if (ok && chnIndex >= 0 && chnIndex <= m_pecalcsettings->getNumber()) {
                    quint32 reg;
                    AbstractDeviceNodeSecPtr deviceNode = m_zdspSupportFactory->getSecDeviceNode();
                    deviceNode->lseek(m_nMyAdress + (ECALCREG::CONF << 2));
                    deviceNode->read(reinterpret_cast<char*>(&reg), 4);
                    reg = (reg & 0xFFFFFF00) | (chnIndex+1);
                    deviceNode->write(reinterpret_cast<char*>(&reg), 4);
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
            if (m_pInputSettings->hasInput(par)) {
                quint32 reg;
                AbstractDeviceNodeSecPtr deviceNode = m_zdspSupportFactory->getSecDeviceNode();
                deviceNode->lseek(m_nMyAdress + (ECALCREG::CONF << 2));
                deviceNode->read(reinterpret_cast<char*>(&reg), 4);
                reg = (reg & 0xFFFF83FF) | (m_pInputSettings->mux(par) << 10);
                deviceNode->write(reinterpret_cast<char*>(&reg), 4);
                protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::ack];
            }
            else
                protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::errval];
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
            bool ok;
            int cmdId = par.toInt(&ok);
            if (ok && cmdId >= 0 && cmdId < SecConfigCount) {
                quint32 reg;
                AbstractDeviceNodeSecPtr deviceNode = m_zdspSupportFactory->getSecDeviceNode();
                deviceNode->lseek(m_nMyAdress + (ECALCREG::CONF << 2));
                deviceNode->read(reinterpret_cast<char*>(&reg), 4);
                reg = (reg & 0x00007CFF) | SecConfigs[cmdId];
                deviceNode->write(reinterpret_cast<char*>(&reg), 4);
                protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::ack];
            }
            else
                protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::errval];
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
            deviceNode->write(reinterpret_cast<char*>(&reg), 4);
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
            if(ok)
                resetInterrupt(interrupt);
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
    deviceNode->write(reinterpret_cast<char*>(&reg), 4);
    // we must reset the stop bit in command register because ecalculator will never start
    reg = 0x0;
    deviceNode->write(reinterpret_cast<char*>(&reg), 4);
}


void SecChannel::resetInterrupt(quint8 interrupt)
{
    quint32 reg;
    notifierECalcChannelIntReg.clrValue(interrupt);
    AbstractDeviceNodeSecPtr deviceNode = m_zdspSupportFactory->getSecDeviceNode();
    deviceNode->lseek(m_nMyAdress + (ECALCREG::INTREG << 2));
    deviceNode->read(reinterpret_cast<char*>(&reg), 4);
    reg = (reg & 0xF) & ~interrupt;
    deviceNode->write(reinterpret_cast<char*>(&reg), 4);
}
