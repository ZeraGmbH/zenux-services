#include "dspinterface_p.h"
#include "dspinterface.h"
#include "variantconverter.h"
#include <netmessages.pb.h>

namespace Zera {

cDSPInterfacePrivate::cDSPInterfacePrivate(cDSPInterface *iface) :
    q_ptr(iface)
{
}

cDSPInterfacePrivate::~cDSPInterfacePrivate()
{
    for(const auto dspMem : qAsConst(m_DspMemoryDataList))
        delete dspMem;
}

void cDSPInterfacePrivate::setClient(Zera::ProxyClient *client)
{
    if (m_pClient) // we avoid multiple connections
        disconnect(m_pClient, 0, this, 0);
    m_pClient = client;
    connect(m_pClient, &Zera::ProxyClient::answerAvailable, this, &cDSPInterfacePrivate::receiveAnswer);
    connect(m_pClient, &Zera::ProxyClient::tcpError, this, &cDSPInterfacePrivate::receiveError);
}

void cDSPInterfacePrivate::setClientSmart(ProxyClientPtr client)
{
    m_clientSmart = client;
    setClient(client.get());
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hint: to find server SCPI implementation, search for last node name in 'long' comments
/////////////////////////////////////////////////////////////////////////////////////////

quint32 cDSPInterfacePrivate::bootDsp()
{
    quint32 msgnr = sendCommand(QString("SYST:DSP:BOOT:EXEC")); // long SYSTEM:DSP:BOOT:EXECUTE
    m_MsgNrCmdList[msgnr] = bootdsp;
    return msgnr;
}

quint32 cDSPInterfacePrivate::resetDsp()
{
    quint32 msgnr = sendCommand(QString("SYST:DSP:RES")); // long SYSTEM:DSP:RESET
    m_MsgNrCmdList[msgnr] = resetdsp;
    return msgnr;
}

quint32 cDSPInterfacePrivate::setSamplingSystem(int chncount, int samp_per, int samp_mper)
{
    quint32 msgnr = sendCommand("SYST:DSP:SAMP", QString("%1,%2,%3").arg(chncount).arg(samp_per).arg(samp_mper)); // long SYSTEM:DSP:SAMPLING
    m_MsgNrCmdList[msgnr] = setsamplingsystem;
    return msgnr;
}

QString cDSPInterfacePrivate::varList2String() const
{
    QString varList;
    QTextStream ts(&varList, QIODevice::WriteOnly);
    cDspMeasData* pDspMeasData;
    for (int i = 0; i < m_DspMemoryDataList.count(); i++) {
        pDspMeasData = m_DspMemoryDataList.at(i);
        ts << pDspMeasData->VarListLong(DSPDATA::vDspParam | DSPDATA::vDspTemp | DSPDATA::vDspResult | DSPDATA::vDspTempGlobal);
    }
    return varList;
}

quint32 cDSPInterfacePrivate::varList2Dsp() // the complete list has several partial lists
{
    QString varList = varList2String();
    quint32 msgnr = sendCommand("MEAS:LIST:RAVL", varList); // long: MEASURE:LIST:RAVLIST
    m_MsgNrCmdList[msgnr] = varlist2dsp;
    return msgnr;
}

quint32 cDSPInterfacePrivate::cmdList2Dsp()
{
    QString plist;
    QTextStream ts( &plist, QIODevice::WriteOnly );
    for ( QStringList::Iterator it = CycCmdList.begin(); it != CycCmdList.end(); ++it )
        ts << *it << ";" ;
    quint32 msgnr = sendCommand("MEAS:LIST:CYCL", plist); // long: MEASURE:LIST:CYCLIST
    m_MsgNrCmdList[msgnr] = cmdlist2dsp;
    return msgnr;
}

quint32 cDSPInterfacePrivate::intList2Dsp()
{
    QString plist;
    QTextStream ts( &plist, QIODevice::WriteOnly );
    for ( QStringList::Iterator it = IntCmdList.begin(); it != IntCmdList.end(); ++it )
        ts << *it << ";" ;
    quint32 msgnr = sendCommand("MEAS:LIST:INTL", plist); // long: MEASURE:LIST:INTLIST
    m_MsgNrCmdList[msgnr] = intlist2dsp;
    return msgnr;
}

int cDSPInterfacePrivate::cmdListCount()
{
    return CycCmdList.count();
}

int cDSPInterfacePrivate::intListCount()
{
    return IntCmdList.count();
}

void cDSPInterfacePrivate::clearCmdList()
{
    CycCmdList.clear();
    IntCmdList.clear();
}

void cDSPInterfacePrivate::clearMemLists()
{
    while ( !m_DspMemoryDataList.isEmpty() )
        delete m_DspMemoryDataList.takeFirst();
}

quint32 cDSPInterfacePrivate::setSignalRouting(tRouting *routingtab)
{
    QString par;
    QTextStream ts(&par, QIODevice::WriteOnly);
    ts << "ETHROUTINGTAB";
    for (uint i = 0; i < (sizeof(tRouting)/sizeof(quint32)); i++)
        ts << "," << routingtab[i];
    quint32 msgnr = sendCommand("MEM:WRIT", par); // long: MEMORY:WRITE ETHROUTINGTAB,...
    m_MsgNrCmdList[msgnr] = setsignalrouting;
    return msgnr;
}

quint32 cDSPInterfacePrivate::setDsp61850PriorityTagged(quint32 priotag)
{
    quint32 msgnr = sendCommand("SYST:DSP:EN61:PRI", QString("%1").arg(priotag));
    m_MsgNrCmdList[msgnr] = setdsp61850prioritytagged;
    return msgnr;
}


quint32 cDSPInterfacePrivate::setDsp61850EthTypeAppId(quint32 typAppid)
{
    quint32 msgnr = sendCommand("SYST:DSP:EN61:ETHT", QString("%1").arg(typAppid));
    m_MsgNrCmdList[msgnr] = setdsp61850ethtypeappid;
    return msgnr;
}

quint32 cDSPInterfacePrivate::setDsp61850EthSynchronisation(quint32 syncdata)
{
    quint32 msgnr = sendCommand("SYST:DSP:EN61:ETHS", QString("%1").arg(syncdata));
    m_MsgNrCmdList[msgnr] = setdsp61850ethsynchronisation;
    return msgnr;
}

quint32 cDSPInterfacePrivate::resetMaximum()
{
    quint32 msgnr = sendCommand("SYST:DSP:MAX:RES");
    m_MsgNrCmdList[msgnr] = resetmaximum;
    return msgnr;
}

quint32 cDSPInterfacePrivate::triggerIntHKSK(quint32 hksk)
{
    quint32 msgnr = sendCommand("SYST:DSP:TRIG:INTL:HKSK", QString("%1").arg(hksk));
    m_MsgNrCmdList[msgnr] = triggerinthksk;
    return msgnr;
}

void cDSPInterfacePrivate::addCycListItem(QString cmd)
{
    CycCmdList.push_back(cmd);
}

void cDSPInterfacePrivate::addCycListItems(const QStringList &cmds)
{
    CycCmdList.append(cmds);
}

void cDSPInterfacePrivate::addIntListItem(QString cmd)
{
    IntCmdList.push_back(cmd);
}

cDspMeasData* cDSPInterfacePrivate::getMemHandle(QString name)
{
    cDspMeasData* pdmd = new cDspMeasData(name);
    m_DspMemoryDataList.append(pdmd);
    return pdmd;
}

cDspMeasData *cDSPInterfacePrivate::findMemHandle(QString name) const
{
    cDspMeasData* memHandleFound = nullptr;
    for(int i=0; i<m_DspMemoryDataList.count(); ++i) {
        if(m_DspMemoryDataList[i]->getName() == name) {
            memHandleFound = m_DspMemoryDataList[i];
            break;
        }
    }
    return memHandleFound;
}

void cDSPInterfacePrivate::deleteMemHandle(cDspMeasData *memhandle)
{
    if (m_DspMemoryDataList.contains(memhandle)) {
        int index = m_DspMemoryDataList.indexOf(memhandle);
        cDspMeasData* pdmd = m_DspMemoryDataList.takeAt(index);
        delete pdmd;
    }
}

quint32 cDSPInterfacePrivate::activateInterface()
{
    quint32 msgnr = sendCommand("MEAS:LIST:SET");
    m_MsgNrCmdList[msgnr] = activateinterface;
    return msgnr;
}

quint32 cDSPInterfacePrivate::deactivateInterface()
{
    quint32 msgnr = sendCommand("MEAS:LIST:CLE");
    m_MsgNrCmdList[msgnr] = deactivateinterface;
    return msgnr;
}

quint32 cDSPInterfacePrivate::dataAcquisition(cDspMeasData *memgroup)
{
    quint32 msgnr = sendCommand(QString("MEAS"), QString("%1").arg(memgroup->VarListShort(DSPDATA::vDspResult)));
    m_MsgNrCmdList[msgnr] = dataacquisition;
    m_MsgNrMeasData[msgnr] = memgroup;
    m_MsgNrMemType[msgnr] = DSPDATA::dFloat;
    return msgnr;
}

quint32 cDSPInterfacePrivate::dspMemoryRead(cDspMeasData *memgroup, DSPDATA::dType type)
{
    quint32 msgnr = sendCommand(QString("MEM:READ"), QString("%1").arg(memgroup->VarListShort(DSPDATA::vDspALL)));
    m_MsgNrCmdList[msgnr] = dspmemoryread;
    m_MsgNrMeasData[msgnr] = memgroup;
    m_MsgNrMemType[msgnr] = type;
    return msgnr;
}

quint32 cDSPInterfacePrivate::dspMemoryWrite(cDspMeasData *memgroup)
{
    quint32 msgnr = sendCommand("MEM:WRIT", memgroup->writeCommand());
    m_MsgNrCmdList[msgnr] = dspmemorywrite;
    return msgnr;
}

quint32 cDSPInterfacePrivate::setGainCorrection(int chn, float val)
{
    quint32 msgnr = sendCommand("MEM:WRIT", QString("GAINCORRECTION+%1,%2").arg(chn).arg(val));
    m_MsgNrCmdList[msgnr] = setgaincorrection;
    return msgnr;
}

quint32 cDSPInterfacePrivate::setPhaseCorrection(int chn, float val)
{
    quint32 msgnr = sendCommand("MEM:WRIT", QString("PHASECORRECTION+%1,%2").arg(chn).arg(val));
    m_MsgNrCmdList[msgnr] = setphasecorrection;
    return msgnr;
}

quint32 cDSPInterfacePrivate::setOffsetCorrection(int chn, float val)
{
    quint32 msgnr = sendCommand("MEM:WRIT", QString("OFFSETCORRECTION+%1,%2").arg(chn).arg(val));
    m_MsgNrCmdList[msgnr] = setoffsetcorrection;
    return msgnr;
}

QStringList cDSPInterfacePrivate::getCyclicCmdList() const
{
    return CycCmdList;
}

QList<cDspMeasData *> cDSPInterfacePrivate::getMemoryDataList() const
{
    return m_DspMemoryDataList;
}

quint32 cDSPInterfacePrivate::readDeviceVersion()
{
    quint32 msgnr = sendCommand("SYST:VERS:DEV?");
    m_MsgNrCmdList[msgnr] = readdeviceversion;
    return msgnr;
}

quint32 cDSPInterfacePrivate::readServerVersion()
{
    quint32 msgnr = sendCommand("SYST:VERS:SERV?");
    m_MsgNrCmdList[msgnr] = readserverversion;
    return msgnr;
}

quint32 cDSPInterfacePrivate::readMaximumLoad()
{
    quint32 msgnr = sendCommand(QString("STATUS:DSP:LOAD:MAXIMUM?"));
    m_MsgNrCmdList[msgnr] = readmaxload;
    return msgnr;
}

quint32 cDSPInterfacePrivate::resetMaximumLoad()
{
    quint32 msgnr = sendCommand(QString("STATUS:DSP:LOAD:MAXIMUM:RESET"));
    m_MsgNrCmdList[msgnr] = resetmaxload;
    return msgnr;
}

void cDSPInterfacePrivate::receiveAnswer(std::shared_ptr<ProtobufMessage::NetMessage> message)
{
    if (message->has_reply()) {
        quint32 lmsgnr = message->messagenr();
        QString lmsg = "";
        if (message->reply().has_body())
            lmsg = QString::fromStdString(message->reply().body());

        quint8 lreply = message->reply().rtype();
        int lastCmd;
        if (lmsgnr == 0)
            lastCmd = dspinterrupt;
        else
            lastCmd = m_MsgNrCmdList.take(lmsgnr);

        Q_Q(cDSPInterface);

        switch (lastCmd)
        {
        case bootdsp:
        case resetdsp:
        case setbootpath:
        case setsamplingsystem:
        case varlist2dsp:
        case cmdlist2dsp:
        case intlist2dsp:
        case setsignalrouting:
        case setdsp61850sourceadr:
        case setdsp61850destadr:
        case setdsp61850prioritytagged:
        case setdsp61850ethtypeappid:
        case setdsp61850ethsynchronisation:
        case resetmaximum:
        case triggerinthksk:
        case activateinterface:
        case deactivateinterface:
        case dspmemorywrite:
        case setgaincorrection:
        case setphasecorrection:
        case setoffsetcorrection:
        case resetmaxload:
            emit q->serverAnswer(lmsgnr, lreply, VariantConverter::returnString(lmsg));
            break;
        case readdeviceversion:
        case readserverversion:
        case dspinterrupt:
        case readmaxload:
            emit q->serverAnswer(lmsgnr, lreply, VariantConverter::returnString(lmsg));
            break;
        case dataacquisition:
        case dspmemoryread: {
            cDspMeasData* actMemGroup = m_MsgNrMeasData.take(lmsgnr);
            DSPDATA::dType actMemType = m_MsgNrMemType.take(lmsgnr);
            Q_UNUSED(actMemType)
            actMemGroup->setVarData(lmsg);
            emit q->serverAnswer(lmsgnr, 0, VariantConverter::returnString(lmsg));
            break;
        }

        }
    }
}


void cDSPInterfacePrivate::receiveError(QAbstractSocket::SocketError errorCode)
{
    Q_Q(cDSPInterface);
    emit q->tcpError(errorCode);
}

}
