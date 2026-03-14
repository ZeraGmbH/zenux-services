#include "pcbinterface_p.h"
#include "pcbinterface.h"
#include "variantconverter.h"
#include <netmessages.pb.h>

namespace Zera {

cPCBInterfacePrivate::cPCBInterfacePrivate(cPCBInterface *iface)
    :q_ptr(iface)
{
}

void cPCBInterfacePrivate::setClientSmart(Zera::ProxyClientPtr client)
{
    if (m_clientSmart) // we avoid multiple connections
        disconnect(m_clientSmart.get(), nullptr, this, nullptr);
    m_clientSmart = client;
    connect(m_clientSmart.get(), &Zera::ProxyClient::answerAvailable, this, &cPCBInterfacePrivate::receiveAnswer);
    connect(m_clientSmart.get(), &Zera::ProxyClient::tcpError, this, &cPCBInterfacePrivate::receiveError);
}

quint32 cPCBInterfacePrivate::getChannelList()
{
    quint32 msgnr = sendCommand(QString("SENSE:CHANNEL:CAT?"));
    m_MsgNrCmdList[msgnr] = PCB::getchannellist;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getDSPChannel(const QString &chnName)
{
    quint32 msgnr = sendCommand(QString("SENS:%1:DSPC?").arg(chnName));
    m_MsgNrCmdList[msgnr] = PCB::getdspchannel;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getStatus(const QString &chnName)
{
    quint32 msgnr = sendCommand(QString("SENS:%1:STAT?").arg(chnName));
    m_MsgNrCmdList[msgnr] = PCB::getstatus;
    return msgnr;
}

quint32 cPCBInterfacePrivate::resetStatus(const QString &chnName)
{
    quint32 msgnr = sendCommand(QString("SENS:%1:STAT:RES;").arg(chnName));
    m_MsgNrCmdList[msgnr] = PCB::resetstatus;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getAlias(const QString &chnName)
{
    quint32 msgnr = sendCommand(QString("SENS:%1:ALI?").arg(chnName));
    m_MsgNrCmdList[msgnr] = PCB::getalias;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getType(const QString &chnName)
{
    quint32 msgnr = sendCommand(QString("SENS:%1:TYPE?").arg(chnName));
    m_MsgNrCmdList[msgnr] = PCB::gettype;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getUnit(const QString &chnName)
{
    quint32 msgnr = sendCommand(QString("SENS:%1:UNIT?").arg(chnName));
    m_MsgNrCmdList[msgnr] = PCB::getunit;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getRange(const QString &chnName)
{
    quint32 msgnr = sendCommand(QString("SENS:%1:RANG?").arg(chnName));
    m_MsgNrCmdList[msgnr] = PCB::getrange;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getRangeList(const QString &chnName)
{
    quint32 msgnr = sendCommand(QString("SENS:%1:RANG:CAT?").arg(chnName));
    m_MsgNrCmdList[msgnr] = PCB::getrangelist;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getAlias(const QString &chnName, const QString &rngName)
{
    quint32 msgnr = sendCommand(QString("SENS:%1:%2:ALI?").arg(chnName, rngName));
    m_MsgNrCmdList[msgnr] = PCB::getalias2;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getType(const QString &chnName, const QString &rngName)
{
    quint32 msgnr = sendCommand(QString("SENS:%1:%2:TYPE?").arg(chnName, rngName));
    m_MsgNrCmdList[msgnr] = PCB::gettype2;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getUrvalue(const QString &chnName, const QString &rngName)
{
    quint32 msgnr = sendCommand(QString("SENS:%1:%2:URV?").arg(chnName, rngName));
    m_MsgNrCmdList[msgnr] = PCB::geturvalue;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getUrvalue(const QString &chnName)
{
    quint32 msgnr = sendCommand(QString("SENS:%1:URV?").arg(chnName));
    m_MsgNrCmdList[msgnr] = PCB::geturvalue2;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getRejection(const QString &chnName, const QString &rngName)
{
    quint32 msgnr = sendCommand(QString("SENS:%1:%2:REJ?").arg(chnName, rngName));
    m_MsgNrCmdList[msgnr] = PCB::getrejection;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getOVRejection(const QString &chnName, const QString &rngName)
{
    quint32 msgnr = sendCommand(QString("SENS:%1:%2:OVR?").arg(chnName, rngName));
    m_MsgNrCmdList[msgnr] = PCB::getovrejection;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getADCRejection(const QString &chnName, const QString &rngName)
{
    quint32 msgnr = sendCommand(QString("SENS:%1:%2:ADCR?").arg(chnName, rngName));
    m_MsgNrCmdList[msgnr] = PCB::getadcrejection;
    return msgnr;
}

quint32 cPCBInterfacePrivate::isAvail(const QString &chnName, const QString &rngName)
{
    quint32 msgnr = sendCommand(QString("SENS:%1:%2:AVA?").arg(chnName, rngName));
    m_MsgNrCmdList[msgnr] = PCB::isavail;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getGainCorrection(const QString &chnName, const QString &rngName, double at)
{
    quint32 msgnr = sendCommand(QString("SENS:%1:%2:CORR:GAIN?").arg(chnName, rngName), QString("%1;").arg(at));
    m_MsgNrCmdList[msgnr] = PCB::getgaincorrection;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getAdjGainCorrection(const QString &chnName, const QString &rngName, double at)
{
    quint32 msgnr = sendCommand(QString("SENS:%1:%2:CORR:ADJG?").arg(chnName, rngName), QString("%1;").arg(at));
    m_MsgNrCmdList[msgnr] = PCB::getgaincorrection;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getOffsetCorrection(const QString &chnName, const QString &rngName, double at)
{
    quint32 msgnr = sendCommand(QString("SENS:%1:%2:CORR:OFFS?").arg(chnName, rngName), QString("%1;").arg(at));
    m_MsgNrCmdList[msgnr] = PCB::getoffsetcorrection;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getAdjOffsetCorrection(const QString &chnName, const QString &rngName, double at)
{
    quint32 msgnr = sendCommand(QString("SENS:%1:%2:CORR:ADJOFFSET?").arg(chnName, rngName), QString("%1;").arg(at));
    m_MsgNrCmdList[msgnr] = PCB::getoffsetcorrection;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getPhaseCorrection(const QString &chnName, const QString &rngName, double at)
{
    quint32 msgnr = sendCommand(QString("SENS:%1:%2:CORR:PHAS?").arg(chnName, rngName), QString("%1;").arg(at));
    m_MsgNrCmdList[msgnr] = PCB::getphasecorrection;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getAdjPhaseCorrection(const QString &chnName, const QString &rngName, double at)
{
    quint32 msgnr = sendCommand(QString("SENS:%1:%2:CORR:ADJP?").arg(chnName, rngName), QString("%1;").arg(at));
    m_MsgNrCmdList[msgnr] = PCB::getphasecorrection;
    return msgnr;
}

quint32 cPCBInterfacePrivate::setRange(const QString &chnName, const QString &rngName)
{
    quint32 msgnr = sendCommand(QString("SENS:%1:RANG").arg(chnName), QString("%1;").arg(rngName));
    m_MsgNrCmdList[msgnr] = PCB::setrange;
    return msgnr;
}

quint32 cPCBInterfacePrivate::setMMode(const QString &mmode)
{
    quint32 msgnr = sendCommand(QString("SENS:MMODE"), QString("%1;").arg(mmode));
    m_MsgNrCmdList[msgnr] = PCB::setmeasuringmode;
    return msgnr;
}

quint32 cPCBInterfacePrivate::adjustComputation()
{
    quint32 msgnr = sendCommand(QString("SENS:CORR:COMP;"));
    m_MsgNrCmdList[msgnr] = PCB::adjustcomputation;
    return msgnr;
}

quint32 cPCBInterfacePrivate::adjustStorage()
{
    quint32 msgnr = sendCommand(QString("SYST:ADJ:FLAS:WRIT;"));
    m_MsgNrCmdList[msgnr] = PCB::adjuststorage;
    return msgnr;
}

quint32 cPCBInterfacePrivate::adjustStorageClamp()
{
    quint32 msgnr = sendCommand(QString("SYST:CLAM:WRIT;"));
    m_MsgNrCmdList[msgnr] = PCB::adjuststorageclamp;
    return msgnr;
}

quint32 cPCBInterfacePrivate::setAdjustGainStatus(const QString &chnName, const QString &rngName, int stat)
{
    quint32 msgnr = sendCommand(QString("SENS:%1:%2:corr:gain:stat").arg(chnName, rngName),
                                QString("%1;").arg(stat));
    m_MsgNrCmdList[msgnr] = PCB::setadjustgainstatus;
    return msgnr;
}

quint32 cPCBInterfacePrivate::setAdjustPhaseStatus(const QString &chnName, const QString &rngName, int stat)
{
    quint32 msgnr = sendCommand(QString("SENS:%1:%2:corr:phas:stat").arg(chnName, rngName),
                                QString("%1;").arg(stat));
    m_MsgNrCmdList[msgnr] = PCB::setadjustphasestatus;
    return msgnr;
}

quint32 cPCBInterfacePrivate::setAdjustOffsetStatus(const QString &chnName, const QString &rngName, int stat)
{
    quint32 msgnr = sendCommand(QString("SENS:%1:%2:corr:offs:stat").arg(chnName, rngName),
                                QString("%1;").arg(stat));
    m_MsgNrCmdList[msgnr] = PCB::setadjustoffsetstatus;
    return msgnr;
}

quint32 cPCBInterfacePrivate::setGainNode(const QString &chnName, const QString &rngName, int nr, double corr, double at)
{
    quint32 msgnr = sendCommand(QString("SENS:%1:%2:corr:gain:node:%3").arg(chnName, rngName).arg(nr),
                                QString("%1;%2;").arg(corr).arg(at));
    m_MsgNrCmdList[msgnr] = PCB::setgainnode;
    return msgnr;
}

quint32 cPCBInterfacePrivate::setPhaseNode(const QString &chnName, const QString &rngName, int nr, double corr, double at)
{
    quint32 msgnr = sendCommand(QString("SENS:%1:%2:corr:phas:node:%3").arg(chnName, rngName).arg(nr),
                                QString("%1;%2;").arg(corr).arg(at));
    m_MsgNrCmdList[msgnr] = PCB::setphasenode;
    return msgnr;
}

quint32 cPCBInterfacePrivate::setOffsetNode(const QString &chnName, const QString &rngName, int nr, double corr, double at)
{
    quint32 msgnr = sendCommand(QString("SENS:%1:%2:corr:offs:node:%3").arg(chnName, rngName).arg(nr),
                                QString("%1;%2;").arg(corr).arg(at));
    m_MsgNrCmdList[msgnr] = PCB::setoffsetnode;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getAliasSource(const QString &chnName)
{
    quint32 msgnr = sendCommand(QString("SOUR:%1:ALI?").arg(chnName));
    m_MsgNrCmdList[msgnr] = PCB::getaliassource;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getDSPChannelSource(const QString &chnName)
{
    quint32 msgnr = sendCommand(QString("SOUR:%1:DSPC?").arg(chnName));
    m_MsgNrCmdList[msgnr] = PCB::getdspchannelsource;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getFormFactorSource(const QString &chnName)
{
    quint32 msgnr = sendCommand(QString("SOUR:%1:FFAC?").arg(chnName));
    m_MsgNrCmdList[msgnr] = PCB::getformfactorsource;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getConstantSource(const QString &chnName)
{
    quint32 msgnr = sendCommand(QString("SOUR:%1:CONS?").arg(chnName));
    m_MsgNrCmdList[msgnr] = PCB::getconstantsource;
    return msgnr;
}

quint32 cPCBInterfacePrivate::setConstantSource(const QString &chnName, double constant)
{
    quint32 msgnr = sendCommand(QString("SOUR:%1:CONS").arg(chnName), QString("%1;").arg(constant));
    m_MsgNrCmdList[msgnr] = PCB::setconstantsource;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getPowTypeSource(const QString &chnName)
{
    quint32 msgnr = sendCommand(QString("SOUR:%1:POWT?").arg(chnName));
    m_MsgNrCmdList[msgnr] = PCB::getpowtypesource;
    return msgnr;
}

quint32 cPCBInterfacePrivate::setPowTypeSource(const QString &chnName, const QString &ptype)
{
    quint32 msgnr = sendCommand(QString("SOUR:%1:POWT").arg(chnName), QString("%1;").arg(ptype));
    m_MsgNrCmdList[msgnr] = PCB::setpowtypesource;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getSampleRate()
{
    quint32 msgnr = sendCommand(QString("SAMP:SRAT?"));
    m_MsgNrCmdList[msgnr] = PCB::getsamplerate;
    return msgnr;
}

quint32 cPCBInterfacePrivate::setPLLChannel(const QString &samplechnName, const QString &pllchnName)
{
    quint32 msgnr = sendCommand(QString("SAMP:%1:PLL").arg(samplechnName), QString("%1;").arg(pllchnName));
    m_MsgNrCmdList[msgnr] = PCB::setpllchannel;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getAliasSchead(const QString &chnName)
{
    quint32 msgnr = sendCommand(QString("SCHEAD:%1:ALIAS?").arg(chnName));
    m_MsgNrCmdList[msgnr] = PCB::getaliassschead;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getAliasFrqinput(const QString &chnName)
{
    quint32 msgnr = sendCommand(QString("FRQINPUT:%1:ALIAS?").arg(chnName));
    m_MsgNrCmdList[msgnr] = PCB::getaliasfrqinput;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getAdjustmentStatus()
{
    quint32 msgnr = sendCommand(QString("STAT:ADJ?"));
    m_MsgNrCmdList[msgnr] = PCB::getadjustmentstatus;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getAuthorizationStatus()
{
    quint32 msgnr = sendCommand(QString("STAT:AUTH?"));
    m_MsgNrCmdList[msgnr] = PCB::getauthorizationstatus;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getAdjustmentChksum()
{
    quint32 msgnr = sendCommand(QString("SYST:ADJ:FLAS:CHKS?"));
    m_MsgNrCmdList[msgnr] = PCB::getadjustmentchksum;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getPCBAdjustmentData()
{
    quint32 msgnr = sendCommand(QString("SYST:ADJ:XML?"));
    m_MsgNrCmdList[msgnr] = PCB::getadjustpcbxml;
    return msgnr;
}

quint32 cPCBInterfacePrivate::setPCBAdjustmentData(const QString &xmlpcb)
{
    quint32 msgnr = sendCommand(QString("SYST:ADJ:XML"), xmlpcb);
    m_MsgNrCmdList[msgnr] = PCB::setadjustpcbxml;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getClampAdjustmentData()
{
    quint32 msgnr = sendCommand(QString("SYST:ADJ:CLAM:XML?"));
    m_MsgNrCmdList[msgnr] = PCB::getadjustclampxml;
    return msgnr;
}

quint32 cPCBInterfacePrivate::setClampAdjustmentData(const QString &xmlclamp)
{
    quint32 msgnr = sendCommand(QString("SYST:ADJ:CLAM:XML"), xmlclamp);
    m_MsgNrCmdList[msgnr] = PCB::setadjustclampxml;
    return msgnr;
}

quint32 cPCBInterfacePrivate::readChannelsConnected()
{
    quint32 msgnr = sendCommand(QString("SYST:EMOB:CHAN?"));
    m_MsgNrCmdList[msgnr] = PCB::getChannelsConnected;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getAccumulatorStatus()
{
    quint32 msgnr = sendCommand(QString("SYST:ACC:STAT?"));
    m_MsgNrCmdList[msgnr] = PCB::getaccumulatorstatus;
    return msgnr;
}

quint32 cPCBInterfacePrivate::getAccuStateOfCharge()
{
    quint32 msgnr = sendCommand(QString("SYST:ACC:SOC?"));
    m_MsgNrCmdList[msgnr] = PCB::getaccumulatorsoc;
    return msgnr;
}

quint32 cPCBInterfacePrivate::readData()
{
    quint32 msgnr = sendCommand(QString("SYST:EMOB:READDATA?"));
    m_MsgNrCmdList[msgnr] = PCB::readData;
    return msgnr;
}

quint32 cPCBInterfacePrivate::writeData(const QString &par)
{
    QString cmd = par == "" ? "SYST:EMOB:WRITEDATA;" : QString("SYST:EMOB:WRITEDATA %1;").arg(par);
    quint32 msgnr = sendCommand(cmd);
    m_MsgNrCmdList[msgnr] = PCB::writeData;
    return msgnr;
}

quint32 cPCBInterfacePrivate::transparentCommand(const QString &cmd)
{
    quint32 msgnr;
    QList<QString> sl = cmd.split(' ');
    if (sl.count() <= 1)
        msgnr = sendCommand(cmd);
    else
        msgnr = sendCommand(sl.at(0), sl.at(1));

    m_MsgNrCmdList[msgnr] = PCB::transparentcommand;
    return msgnr;
}

quint32 cPCBInterfacePrivate::resourceAliasQuery(const QString &resourceType, const QString &resourceName)
{
    quint32 msgnr = sendCommand(QString("%1:%2:ALI?").arg(resourceType, resourceName));
    m_MsgNrCmdList[msgnr] = PCB::resourcealiasquery;
    return msgnr;
}

quint32 cPCBInterfacePrivate::registerNotifier(const QString &query, int notifyId, bool notifyWithValue)
{
    quint32 msgnr = sendCommand(QString("SERV:REG"), QString("%1;%2;%3;").arg(query).arg(notifyId).arg(notifyWithValue));
    m_MsgNrCmdList[msgnr] = PCB::regnotifier;
    return msgnr;
}

quint32 cPCBInterfacePrivate::unregisterNotifiers()
{
    quint32 msgnr = sendCommand(QString("SERV:UNR;"));
    m_MsgNrCmdList[msgnr] = PCB::unregnotifier;
    return msgnr;
}

quint32 cPCBInterfacePrivate::readServerVersion()
{
    quint32 msgnr = sendCommand(QString("SYST:VERS:SERV?"));
    m_MsgNrCmdList[msgnr] = PCB::getserverversion;
    return msgnr;
}

quint32 cPCBInterfacePrivate::readPCBInfo()
{
    quint32 msgnr = sendCommand(QString("SYST:VERS:PCB?"));
    m_MsgNrCmdList[msgnr] = PCB::getpcbversion;
    return msgnr;
}

quint32 cPCBInterfacePrivate::readFPGAVersion()
{
    quint32 msgnr = sendCommand(QString("SYST:VERS:FPGA?"));
    m_MsgNrCmdList[msgnr] = PCB::getfpgaversion;
    return msgnr;
}

quint32 cPCBInterfacePrivate::readCTRLVersion()
{
    quint32 msgnr = sendCommand(QString("SYST:VERS:CTRL?"));
    m_MsgNrCmdList[msgnr] = PCB::getctrlversion;
    return msgnr;
}

quint32 cPCBInterfacePrivate::readSerialNr()
{
    quint32 msgnr = sendCommand(QString("SYST:SER?"));
    m_MsgNrCmdList[msgnr] = PCB::getserialnumber;
    return msgnr;
}

quint32 cPCBInterfacePrivate::writeSerialNr(const QString &serNr)
{
    quint32 msgnr = sendCommand(QString("SYST:SER"), QString("%1;").arg(serNr));
    m_MsgNrCmdList[msgnr] = PCB::setserialnumber;
    return msgnr;
}

void cPCBInterfacePrivate::receiveAnswer(std::shared_ptr<ProtobufMessage::NetMessage> message)
{
    TAnswerDecoded decodedAnswer;
    if(decodeProtobuffAnswer(message, decodedAnswer, PCB::pcbinterrupt)) {
        Q_Q(cPCBInterface);
        switch (decodedAnswer.cmdSendEnumVal)
        {
        case PCB::getdspchannel:
        case PCB::getstatus:
        case PCB::gettype2:
        case PCB::getsamplerate:
        case PCB::getdspchannelsource:
        case PCB::getadjustmentstatus:
        case PCB::getauthorizationstatus:
        case PCB::getaccumulatorstatus:
        case PCB::getaccumulatorsoc:
            emit q->serverAnswer(decodedAnswer.msgNr, decodedAnswer.reply, VariantConverter::returnInt(decodedAnswer.msgBody));
            break;

        case PCB::getalias:
        case PCB::gettype:
        case PCB::getunit:
        case PCB::getrange:
        case PCB::getalias2:
        case PCB::getaliassource:
        case PCB::getaliassschead:
        case PCB::getaliasfrqinput:
        case PCB::resourcealiasquery:
        case PCB::getserverversion:
        case PCB::getpcbversion:
        case PCB::getfpgaversion:
        case PCB::getctrlversion:
        case PCB::getserialnumber:
        case PCB::getadjustmentchksum:
        case PCB::getadjustpcbxml:
        case PCB::getadjustclampxml:
        case PCB::getpcberrorstatus:
        case PCB::getpowtypesource:
        case PCB::transparentcommand:
            emit q->serverAnswer(decodedAnswer.msgNr, decodedAnswer.reply, VariantConverter::returnString(decodedAnswer.msgBody));
            break;

        case PCB::getchannellist:
        case PCB::getrangelist:
            emit q->serverAnswer(decodedAnswer.msgNr, decodedAnswer.reply, VariantConverter::returnStringList(decodedAnswer.msgBody));
            break;

        case PCB::geturvalue:
        case PCB::geturvalue2:
        case PCB::getrejection:
        case PCB::getovrejection:
        case PCB::getadcrejection:
        case PCB::getgaincorrection:
        case PCB::getoffsetcorrection:
        case PCB::getphasecorrection:
        case PCB::getformfactorsource:
        case PCB::getconstantsource:
            emit q->serverAnswer(decodedAnswer.msgNr, decodedAnswer.reply, VariantConverter::returnDouble(decodedAnswer.msgBody));
            break;

        case PCB::isavail:
            emit q->serverAnswer(decodedAnswer.msgNr, decodedAnswer.reply, VariantConverter::returnBool(decodedAnswer.msgBody));
            break;

        case PCB::setoffsetnode:
        case PCB::setphasenode:
        case PCB::setgainnode:
        case PCB::adjuststorage:
        case PCB::adjuststorageclamp:
        case PCB::adjustinit:
        case PCB::setadjustgainstatus:
        case PCB::setadjustphasestatus:
        case PCB::setadjustoffsetstatus:
        case PCB::adjustcomputation:
        case PCB::setmeasuringmode:
        case PCB::setrange:
        case PCB::resetstatus:
        case PCB::regnotifier:
        case PCB::unregnotifier:
        case PCB::pcbinterrupt:
        case PCB::setpllchannel:
        case PCB::setconstantsource:
        case PCB::setpowtypesource:
        case PCB::setadjustpcbxml:
        case PCB::setadjustclampxml:
        case PCB::setserialnumber:
        case PCB::getChannelsConnected:
        case PCB::readData:
        case PCB::writeData:
            emit q->serverAnswer(decodedAnswer.msgNr, decodedAnswer.reply, VariantConverter::returnString(decodedAnswer.msgBody));
            break;
        }
    } // hmm ... we have to look what to do otherwise
}


void cPCBInterfacePrivate::receiveError(QAbstractSocket::SocketError errorCode)
{
    Q_Q(cPCBInterface);
    emit q->tcpError(errorCode);
}

}

